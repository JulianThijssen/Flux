#include "DeferredRenderer.h"

#include <glad/glad.h>

#include "Renderer/AveragePass.h"
#include "Renderer/MultiplyPass.h"
#include "Renderer/SSAOPass.h"
#include "Renderer/SkyPass.h"
#include "Renderer/BloomPass.h"
#include "Renderer/GaussianBlurPass.h"
#include "Renderer/TonemapPass.h"
#include "Renderer/DirectLightPass.h"
#include "Renderer/GammaCorrectionPass.h"
#include "Renderer/FxaaPass.h"
#include "Renderer/ColorGradingPass.h"

#include "Transform.h"
#include "Camera.h"
#include "AttachedTo.h"
#include "MeshRenderer.h"
#include "AssetManager.h"
#include "TextureLoader.h"
#include "TextureUnit.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "Path.h"
#include "Size.h"

#include <iostream>

#include "Matrix4f.h"
#include "nvToolsExt.h"

//#define DEBUG_MODE

#ifdef DEBUG_MODE
#define LOG(str) do { Log::debug(str); } while(false)
#else
#define LOG(str) do { } while(false)
#endif

namespace Flux {
    bool DeferredRenderer::create(const Scene& scene, const Size windowSize) {
        addShader(IBL, Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/IBL.frag"));
        addShader(TEXTURE, Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Texture.frag"));
        addShader(GBUFFER, Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/GBuffer.frag"));
        addShader(DINDIRECT, Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/DeferredIndirect.frag"));
        addShader(SHADOW, Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/Shadow.frag"));

        if (!validateShaders()) {
            return false;
        }

        createShadowMaps(scene);

        if (scene.skybox) {
            iblSceneInfo.PrecomputeEnvironmentData(*scene.skybox);
        }
        else if (scene.skySphere) {
            iblSceneInfo.PrecomputeEnvironmentData(*scene.skySphere);
        }

        ssaoInfo.generate();

        averagePass = new AveragePass();
        multiplyPass = new MultiplyPass();
        ssaoPass = new SSAOPass();
        skyPass = new SkyPass();
        bloomPass = new BloomPass();
        gaussianBlurPass = new GaussianBlurPass();
        tonemapPass = new TonemapPass();
        directLightPass = new DirectLightPass();
        gammaCorrectionPass = new GammaCorrectionPass();
        fxaaPass = new FxaaPass();
        colorGradingPass = new ColorGradingPass();

        enable(DEPTH_TEST);
        enable(FACE_CULLING);

        return true;
    }

    void DeferredRenderer::createBackBuffers(const unsigned int width, const unsigned int height) {
        hdrBuffer = new Framebuffer(windowSize.width, windowSize.height);
        hdrBuffer->bind();
        hdrBuffer->addColorTexture(0, TextureLoader::createEmpty(windowSize.width, windowSize.height, GL_RGBA16F, GL_RGBA, GL_FLOAT, Sampling::NEAREST, true));
        hdrBuffer->addDepthTexture(gBuffer.depthTex);
        hdrBuffer->validate();
        hdrBuffer->release();

        for (int i = 0; i < 2; i++) {
            Framebuffer framebuffer(windowSize.width, windowSize.height);
            framebuffer.bind();
            framebuffer.addColorTexture(0, TextureLoader::createEmpty(windowSize.width, windowSize.height, GL_RGBA16F, GL_RGBA, GL_FLOAT, Sampling::LINEAR, true));
            framebuffer.validate();
            framebuffer.release();
            hdrBackBuffers.push_back(framebuffer);
        }
        for (int i = 0; i < 2; i++) {
            Framebuffer framebuffer(windowSize.width, windowSize.height);
            framebuffer.bind();
            framebuffer.addColorTexture(0, TextureLoader::createEmpty(windowSize.width, windowSize.height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, Sampling::LINEAR, true));
            framebuffer.validate();
            framebuffer.release();
            backBuffers.push_back(framebuffer);
        }
    }

    void DeferredRenderer::createShadowMaps(const Scene& scene) {
        shadowBuffer = std::make_unique<Framebuffer>(4096, 4096);
        shadowBuffer->bind();
        shadowBuffer->disableColor();
        shadowBuffer->release();

        for (Entity* entity : scene.lights) {
            Transform* t = entity->getComponent<Transform>();
            Camera* camera = entity->getComponent<Camera>();
            DirectionalLight* light = entity->getComponent<DirectionalLight>();

            light->shadowMap = TextureLoader::createShadowMap(4096, 4096);
        }
    }

    void DeferredRenderer::onResize(const Size windowSize) {
        this->windowSize.setSize(windowSize.width, windowSize.height);

        gBuffer.create(windowSize.width, windowSize.height);
        createBackBuffers(windowSize.width, windowSize.height);

        // Generate half sized framebuffers for low-resolution SSAO rendering
        ssaoInfo.createBuffers(windowSize.width, windowSize.height);

        gaussianBlurPass->Resize(windowSize);
        fxaaPass->Resize(windowSize);

        setClearColor(1.0, 0.0, 1.0, 1.0);
    }

    void DeferredRenderer::update(const Scene& scene) {
        LOG("Updating");
        if (scene.getMainCamera() == nullptr)
            return;

        renderShadowMaps(scene);

        glViewport(0, 0, windowSize.width, windowSize.height);
        LOG("Rendering GBuffer");
        gBuffer.bind();
        setShader(GBUFFER);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        setCamera(*scene.getMainCamera());
        nvtxRangePushA("GBuffer");
        renderScene(scene);
        nvtxRangePop();
        LOG("Finished GBuffer");

        glDepthMask(false);

        globalIllumination(scene);
        directLighting(scene);
        glDepthMask(true);
        skyPass->render(scene);
        applyPostprocess(scene);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_DEPTH_BUFFER_BIT);
        renderFramebuffer(getCurrentFramebuffer());
        LOG("Done updating");
    }

    void DeferredRenderer::globalIllumination(const Scene& scene) {
        switchHdrBuffers();

        glClear(GL_COLOR_BUFFER_BIT);
        LOG("Indirect lighting");
        nvtxRangePushA("Indirect");
        setShader(DINDIRECT);

        setCamera(*scene.getMainCamera());

        gBuffer.albedoTex->bind(TextureUnit::ALBEDO);
        shader->uniform1i("albedoMap", TextureUnit::ALBEDO);
        gBuffer.normalTex->bind(TextureUnit::NORMAL);
        shader->uniform1i("normalMap", TextureUnit::NORMAL);
        gBuffer.positionTex->bind(TextureUnit::POSITION);
        shader->uniform1i("positionMap", TextureUnit::POSITION);
        gBuffer.depthTex->bind(TextureUnit::DEPTH);
        shader->uniform1i("depthMap", TextureUnit::DEPTH);

        iblSceneInfo.irradianceMap->bind(TextureUnit::IRRADIANCE);
        shader->uniform1i("irradianceMap", TextureUnit::IRRADIANCE);

        iblSceneInfo.prefilterEnvmap->bind(TextureUnit::PREFILTER);
        shader->uniform1i("prefilterEnvmap", TextureUnit::PREFILTER);

        iblSceneInfo.scaleBiasTexture->bind(TextureUnit::SCALEBIAS);
        shader->uniform1i("scaleBiasMap", TextureUnit::SCALEBIAS);

        drawQuad();

        switchBuffers();

        ssao(scene);
        multiply(scene);

        nvtxRangePop();
    }

    void DeferredRenderer::ssao(const Scene& scene) {
        ssaoPass->SetGBuffer(&gBuffer);
        ssaoPass->SetSsaoInfo(&ssaoInfo);
        ssaoPass->SetWindowSize(&windowSize);

        ssaoPass->render(scene);
    }

    void DeferredRenderer::multiply(const Scene& scene) {
        hdrBuffer->bind();

        std::vector<Texture> v = {
            getCurrentHdrFramebuffer().getColorTexture(0),
            ssaoInfo.getCurrentBuffer()->getColorTexture(0)
        };
        multiplyPass->SetTextures(v);
        multiplyPass->render(scene);
    }

    void DeferredRenderer::directLighting(const Scene& scene) {
        directLightPass->SetGBuffer(&gBuffer);

        directLightPass->render(scene);
    }

    void DeferredRenderer::renderScene(const Scene& scene) {
        for (Entity* e : scene.entities) {
            if (!e->hasComponent<Mesh>())
                continue;

            if (e->hasComponent<MeshRenderer>()) {
                MeshRenderer* mr = e->getComponent<MeshRenderer>();
                Material* material = scene.materials[mr->materialID];

                if (material) {
                    material->bind(*shader);

                    renderMesh(scene, e);

                    material->release(*shader);
                }
            }

            //renderMesh(scene, e);
        }
    }

    void DeferredRenderer::renderMesh(const Scene& scene, Entity* e) {
        nvtxRangePushA("Mesh");
        Transform* transform = e->getComponent<Transform>();
        Mesh* mesh = e->getComponent<Mesh>();

        modelMatrix.setIdentity();

        if (e->hasComponent<AttachedTo>()) {
            Entity* parent = scene.getEntityById(e->getComponent<AttachedTo>()->parentId);

            if (parent != nullptr) {
                Transform* parentT = parent->getComponent<Transform>();
                modelMatrix.translate(parentT->position);
                modelMatrix.rotate(parentT->rotation);
                modelMatrix.scale(parentT->scale);
            }
        }

        modelMatrix.translate(transform->position);
        modelMatrix.rotate(transform->rotation);
        modelMatrix.scale(transform->scale);

        Matrix4f PVM = projMatrix * viewMatrix * modelMatrix;
        shader->uniformMatrix4f("modelMatrix", modelMatrix);
        shader->uniformMatrix4f("PVM", PVM);

        glBindVertexArray(mesh->handle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, 0);
        nvtxRangePop();
    }

    void DeferredRenderer::applyPostprocess(const Scene& scene) {
        LOG("Post-processing");
        nvtxRangePushA("Post-process");

        bloom(scene);
        blur(scene);
        tonemap(scene);
        gammaCorrection(scene);
        antiAliasing(scene);
        colorGrading(scene);

        nvtxRangePop();
    }

    void DeferredRenderer::bloom(const Scene& scene) {
        bloomPass->SetSource(&hdrBuffer->getColorTexture(0));
        bloomPass->SetTarget(&getCurrentHdrFramebuffer());

        bloomPass->render(scene);
    }

    void DeferredRenderer::blur(const Scene& scene) {
        gaussianBlurPass->SetSource(&getCurrentHdrFramebuffer().getColorTexture(0));
        gaussianBlurPass->SetTarget(&getCurrentHdrFramebuffer());

        gaussianBlurPass->render(scene);
    }

    void DeferredRenderer::tonemap(const Scene& scene) {
        tonemapPass->SetSource(&hdrBuffer->getColorTexture(0));
        tonemapPass->SetBloom(&getCurrentHdrFramebuffer().getColorTexture(0));
        tonemapPass->SetTarget(&getCurrentFramebuffer());

        tonemapPass->render(scene);
    }

    void DeferredRenderer::gammaCorrection(const Scene& scene) {
        gammaCorrectionPass->SetSource(&getCurrentFramebuffer().getColorTexture(0));
        switchBuffers();
        gammaCorrectionPass->SetTarget(&getCurrentFramebuffer());

        gammaCorrectionPass->render(scene);
    }

    void DeferredRenderer::antiAliasing(const Scene& scene) {
        fxaaPass->SetSource(&getCurrentFramebuffer().getColorTexture(0));
        switchBuffers();
        fxaaPass->SetTarget(&getCurrentFramebuffer());

        fxaaPass->render(scene);
    }

    void DeferredRenderer::colorGrading(const Scene& scene) {
        colorGradingPass->SetSource(&getCurrentFramebuffer().getColorTexture(0));
        switchBuffers();
        colorGradingPass->SetTarget(&getCurrentFramebuffer());

        colorGradingPass->render(scene);
    }

    void DeferredRenderer::renderDepth(const Scene& scene) {
        nvtxRangePushA("Depth");

        setShader(SHADOW);

        glClear(GL_DEPTH_BUFFER_BIT);
        glColorMask(false, false, false, false);

        setCamera(*scene.getMainCamera());
        renderScene(scene);

        glColorMask(true, true, true, true);

        nvtxRangePop();
    }

    void DeferredRenderer::renderShadowMaps(const Scene& scene) {
        nvtxRangePushA("Shadow");

        setShader(SHADOW);

        shadowBuffer->bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        glColorMask(false, false, false, false);

        glPolygonOffset(2.5f, 10.0f);
        enable(POLYGON_OFFSET);

        for (Entity* entity : scene.lights) {
            Transform* t = entity->getComponent<Transform>();
            Camera* camera = entity->getComponent<Camera>();
            DirectionalLight* light = entity->getComponent<DirectionalLight>();

            setCamera(*entity);

            light->shadowSpace = Matrix4f::BIAS * projMatrix * viewMatrix;

            shadowBuffer->addDepthTexture(light->shadowMap);
            glViewport(0, 0, light->shadowMap->getWidth(), light->shadowMap->getHeight());

            renderScene(scene);
        }
        disable(POLYGON_OFFSET);

        glColorMask(true, true, true, true);

        nvtxRangePop();
    }

    void DeferredRenderer::renderFramebuffer(const Framebuffer& framebuffer) {
        LOG("Rendering framebuffer");
        setShader(TEXTURE);
        framebuffer.getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        drawQuad();
    }
}

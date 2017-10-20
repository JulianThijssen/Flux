#include "DeferredRenderer.h"

#include <glad/glad.h>

#include "Renderer/AveragePass.h"
#include "Renderer/MultiplyPass.h"
#include "Renderer/SSAOPass.h"
#include "Renderer/SkyPass.h"

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
        addShader(DIRECT, Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/Lighting.frag"));
        addShader(TEXTURE, Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Texture.frag"));
        addShader(FXAA, Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/FXAAQuality.frag"));
        addShader(GAMMA, Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/GammaCorrection.frag"));
        addShader(TONEMAP, Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Tonemap.frag"));
        addShader(BLOOM, Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Bloom.frag"));
        addShader(BLUR, Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/BlurFast.frag"));
        addShader(GBUFFER, Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/GBuffer.frag"));
        addShader(DINDIRECT, Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/DeferredIndirect.frag"));
        addShader(DDIRECT, Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/DeferredDirect.frag"));
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

        int blurWidth = windowSize.width;
        int blurHeight = windowSize.height;
        blurBuffers.resize(4);
        blurBuffers2.resize(4);
        for (unsigned int i = 0; i < blurBuffers.size(); i++) {
            blurWidth = blurWidth >> 1; blurHeight = blurHeight >> 1;
            blurBuffers[i] = new Framebuffer(blurWidth, blurHeight);
            blurBuffers[i]->bind();
            blurBuffers[i]->addColorTexture(0, TextureLoader::createEmpty(blurWidth, blurHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT, Sampling::LINEAR, false));
            blurBuffers[i]->validate();
            blurBuffers[i]->release();
            blurBuffers2[i] = new Framebuffer(blurWidth, blurHeight);
            blurBuffers2[i]->bind();
            blurBuffers2[i]->addColorTexture(0, TextureLoader::createEmpty(blurWidth, blurHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT, Sampling::LINEAR, false));
            blurBuffers2[i]->validate();
            blurBuffers2[i]->release();
        }

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
        ssaoPass->SetCamera(scene.getMainCamera());

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
        LOG("Direct lighting");
        nvtxRangePushA("Direct");
        enable(BLENDING);
        glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
        glDepthFunc(GL_LEQUAL);

        setShader(DDIRECT);
        setCamera(*scene.getMainCamera());

        gBuffer.albedoTex->bind(TextureUnit::ALBEDO);
        shader->uniform1i("albedoMap", TextureUnit::ALBEDO);
        gBuffer.normalTex->bind(TextureUnit::NORMAL);
        shader->uniform1i("normalMap", TextureUnit::NORMAL);
        gBuffer.positionTex->bind(TextureUnit::POSITION);
        shader->uniform1i("positionMap", TextureUnit::POSITION);

        for (Entity* light : scene.lights) {
            DirectionalLight* directionalLight = light->getComponent<DirectionalLight>();
            PointLight* pointLight = light->getComponent<PointLight>();
            Transform* transform = light->getComponent<Transform>();

            if (directionalLight) {
                shader->uniform3f("dirLight.direction", directionalLight->direction);
                shader->uniform3f("dirLight.color", directionalLight->color);
                shader->uniform1i("isDirLight", true);
                shader->uniform1i("isPointLight", false);
                directionalLight->shadowMap->bind(TextureUnit::SHADOW);
                shader->uniform1i("dirLight.shadowMap", TextureUnit::SHADOW);
                shader->uniformMatrix4f("dirLight.shadowMatrix", directionalLight->shadowSpace);
            }
            else if (pointLight) {
                shader->uniform3f("pointLight.position", transform->position);
                shader->uniform3f("pointLight.color", pointLight->color);
                shader->uniform1i("isPointLight", true);
                shader->uniform1i("isDirLight", false);
            }
            else {
                continue;
            }

            drawQuad();
        }

        disable(BLENDING);
        nvtxRangePop();
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

        bloom();
        blur(scene);
        tonemap();
        gammaCorrection();
        antiAliasing();

        nvtxRangePop();
    }

    void DeferredRenderer::bloom() {
        nvtxRangePushA("Bloom");
        setShader(BLOOM);
        hdrBuffer->getColorTexture(0).bind(TextureUnit::TEXTURE);
        glGenerateMipmap(GL_TEXTURE_2D);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        shader->uniform1f("threshold", 0);
        switchHdrBuffers();
        drawQuad();
        nvtxRangePop();
    }

    void DeferredRenderer::blur(const Scene& scene) {
        nvtxRangePushA("Blur");
        setShader(BLUR);

        getCurrentHdrFramebuffer().getColorTexture(0).bind(TextureUnit::TEXTURE);
        glGenerateMipmap(GL_TEXTURE_2D);

        shader->uniform1i("tex", TextureUnit::TEXTURE);

        shader->uniform2f("direction", 1, 0);
        for (unsigned int i = 0; i < blurBuffers.size(); i++) {
            const Texture& texture = blurBuffers[i]->getColorTexture(0);
            int width = texture.getWidth();
            int height = texture.getHeight();
            glViewport(0, 0, width, height);
            shader->uniform2i("windowSize", width, height);
            blurBuffers[i]->bind();
            shader->uniform1i("mipmap", i + 1);

            drawQuad();
        }
        shader->uniform2f("direction", 0, 1);
        for (unsigned int i = 0; i < blurBuffers2.size(); i++) {
            const Texture& texture = blurBuffers[i]->getColorTexture(0);
            texture.bind(TextureUnit::TEXTURE);
            int width = texture.getWidth();
            int height = texture.getHeight();
            glViewport(0, 0, width, height);
            shader->uniform2i("windowSize", width, height);
            blurBuffers2[i]->bind();
            shader->uniform1i("mipmap", 0);

            drawQuad();
        }
        nvtxRangePop();


        glViewport(0, 0, windowSize.width, windowSize.height);

        switchHdrBuffers();

        std::vector<Texture> v = {
            blurBuffers2[0]->getColorTexture(0),
            blurBuffers2[1]->getColorTexture(0),
            blurBuffers2[2]->getColorTexture(0),
            blurBuffers2[3]->getColorTexture(0),
        };
        averagePass->SetTextures(v);
        averagePass->render(scene);
    }

    void DeferredRenderer::tonemap() {
        nvtxRangePushA("Tonemap");
        setShader(TONEMAP);
        hdrBuffer->getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        getCurrentHdrFramebuffer().getColorTexture(0).bind(TextureUnit::BLOOM);
        shader->uniform1i("bloomTex", TextureUnit::BLOOM);
        switchBuffers();
        drawQuad();
        nvtxRangePop();
    }

    void DeferredRenderer::gammaCorrection() {
        nvtxRangePushA("Gamma");
        setShader(GAMMA);
        getCurrentFramebuffer().getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        switchBuffers();
        drawQuad();
        nvtxRangePop();
    }

    void DeferredRenderer::antiAliasing() {
        nvtxRangePushA("FXAA");
        setShader(FXAA);
        getCurrentFramebuffer().getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        glGenerateMipmap(GL_TEXTURE_2D);
        shader->uniform2f("rcpScreenSize", 1.0f / windowSize.width, 1.0f / windowSize.height);
        switchBuffers();
        drawQuad();
        nvtxRangePop();
    }

    void DeferredRenderer::renderShadowMaps(const Scene& scene) {
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
    }

    void DeferredRenderer::renderFramebuffer(const Framebuffer& framebuffer) {
        LOG("Rendering framebuffer");
        setShader(TEXTURE);
        framebuffer.getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        drawQuad();
    }
}

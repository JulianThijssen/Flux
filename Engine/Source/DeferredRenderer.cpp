#include "DeferredRenderer.h"

#include <glad/glad.h>

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
        addShader(IBL,       Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/IBL.frag"));
        addShader(DIRECT,    Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/Lighting.frag"));
        addShader(SKYBOX,    Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Skybox.frag"));
        addShader(TEXTURE,   Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Texture.frag"));
        addShader(FXAA,      Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/FXAAQuality.frag"));
        addShader(GAMMA,     Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/GammaCorrection.frag"));
        addShader(TONEMAP,   Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Tonemap.frag"));
        addShader(SKYSPHERE, Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Skysphere.frag"));
        addShader(BLOOM,     Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Bloom.frag"));
        addShader(BLUR,      Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Blur.frag"));
        addShader(SSAO,      Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/SSAO.frag"));
        addShader(MULTIPLY,  Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Multiply.frag"));
        addShader(GBUFFER,   Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/GBuffer.frag"));
        addShader(DINDIRECT, Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/DeferredIndirect.frag"));
        addShader(DDIRECT,   Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/DeferredDirect.frag"));
        addShader(SHADOW,    Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/Shadow.frag"));
        addShader(SSAOBLUR,  Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/SSAOBlur.frag"));

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

        ssaoInfo.generate(32, 4);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        return true;
    }

    void DeferredRenderer::createGBuffer(const unsigned int width, const unsigned int height) {
        gBufferInfo.albedoTex = TextureLoader::createEmpty(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, Sampling::NEAREST, false);
        gBufferInfo.normalTex = TextureLoader::createEmpty(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, Sampling::NEAREST, false);
        gBufferInfo.positionTex = TextureLoader::createEmpty(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, Sampling::NEAREST, false);
        gBufferInfo.depthTex = TextureLoader::createEmpty(width, height, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, Sampling::NEAREST, false);

        gBuffer = std::make_unique<Framebuffer>(width, height);
        gBuffer->bind();
        gBuffer->addColorTexture(0, gBufferInfo.albedoTex);
        gBuffer->addColorTexture(1, gBufferInfo.normalTex);
        gBuffer->addColorTexture(2, gBufferInfo.positionTex);
        gBuffer->addDrawBuffer(GL_COLOR_ATTACHMENT0);
        gBuffer->addDrawBuffer(GL_COLOR_ATTACHMENT1);
        gBuffer->addDrawBuffer(GL_COLOR_ATTACHMENT2);
        gBuffer->addDepthTexture(gBufferInfo.depthTex);
        gBuffer->validate();
        gBuffer->release();
    }

    void DeferredRenderer::createBackBuffers(const unsigned int width, const unsigned int height) {
        hdrBuffer = new Framebuffer(windowSize.width, windowSize.height);
        hdrBuffer->bind();
        hdrBuffer->addColorTexture(0, TextureLoader::createEmpty(windowSize.width, windowSize.height, GL_RGBA16F, GL_RGBA, GL_FLOAT, Sampling::NEAREST, true));
        hdrBuffer->addDepthTexture(gBufferInfo.depthTex);
        hdrBuffer->validate();
        hdrBuffer->release();

        for (int i = 0; i < 2; i++) {
            Framebuffer framebuffer(windowSize.width, windowSize.height);
            framebuffer.bind();
            framebuffer.addColorTexture(0, TextureLoader::createEmpty(windowSize.width, windowSize.height, GL_RGBA16F, GL_RGBA, GL_FLOAT, Sampling::NEAREST, true));
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

            light->shadowMap = TextureLoader::createEmpty(4096, 4096, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, Sampling::NEAREST, false);
        }
    }

    void DeferredRenderer::onResize(const Size windowSize) {
        this->windowSize.setSize(windowSize.width, windowSize.height);

        createGBuffer(windowSize.width, windowSize.height);
        createBackBuffers(windowSize.width, windowSize.height);

        setClearColor(1.0, 0.0, 1.0, 1.0);
    }

    void DeferredRenderer::update(const Scene& scene) {
        LOG("Updating");
        if (scene.getMainCamera() == nullptr)
            return;

        renderShadowMaps(scene);

        glViewport(0, 0, windowSize.width, windowSize.height);
        LOG("Rendering GBuffer");
        gBuffer->bind();
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
        renderSky(scene);
        applyPostprocess();
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

        gBuffer->getColorTexture(0).bind(TextureUnit::ALBEDO);
        shader->uniform1i("albedoMap", TextureUnit::ALBEDO);
        gBuffer->getColorTexture(1).bind(TextureUnit::NORMAL);
        shader->uniform1i("normalMap", TextureUnit::NORMAL);
        gBuffer->getColorTexture(2).bind(TextureUnit::POSITION);
        shader->uniform1i("positionMap", TextureUnit::POSITION);
        gBufferInfo.depthTex->bind(TextureUnit::DEPTH);
        shader->uniform1i("depthMap", TextureUnit::DEPTH);

        iblSceneInfo.irradianceMap->bind(TextureUnit::IRRADIANCE);
        shader->uniform1i("irradianceMap", TextureUnit::IRRADIANCE);

        iblSceneInfo.prefilterEnvmap->bind(TextureUnit::PREFILTER);
        shader->uniform1i("prefilterEnvmap", TextureUnit::PREFILTER);

        iblSceneInfo.scaleBiasTexture->bind(TextureUnit::SCALEBIAS);
        shader->uniform1i("scaleBiasMap", TextureUnit::SCALEBIAS);

        drawQuad();

        switchBuffers();

        glClear(GL_COLOR_BUFFER_BIT);
        setShader(SSAO);

        setCamera(*scene.getMainCamera());

        gBuffer->getColorTexture(0).bind(TextureUnit::ALBEDO);
        shader->uniform1i("albedoMap", TextureUnit::ALBEDO);
        gBuffer->getColorTexture(1).bind(TextureUnit::NORMAL);
        shader->uniform1i("normalMap", TextureUnit::NORMAL);
        gBuffer->getColorTexture(2).bind(TextureUnit::POSITION);
        shader->uniform1i("positionMap", TextureUnit::POSITION);
        gBufferInfo.depthTex->bind(TextureUnit::DEPTH);
        shader->uniform1i("depthMap", TextureUnit::DEPTH);

        ssaoInfo.noiseTexture->bind(TextureUnit::NOISE);
        shader->uniform1i("noiseMap", TextureUnit::NOISE);
        shader->uniform3fv("kernel", ssaoInfo.kernel.size(), ssaoInfo.kernel.data());
        shader->uniform1i("kernelSize", ssaoInfo.kernel.size());

        drawQuad();
        nvtxRangePop();

        setShader(SSAOBLUR);
        shader->uniform2f("windowSize", windowSize.width, windowSize.height);

        getCurrentFramebuffer().getColorTexture(0).bind(TextureUnit::TEXTURE);
        glGenerateMipmap(GL_TEXTURE_2D);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        switchBuffers();
        glClear(GL_COLOR_BUFFER_BIT);
        drawQuad();

        hdrBuffer->bind();

        setShader(MULTIPLY);
        getCurrentHdrFramebuffer().getColorTexture(0).bind(TextureUnit::ALBEDO);
        getCurrentFramebuffer().getColorTexture(0).bind(TextureUnit::NORMAL);
        shader->uniform1i("texA", TextureUnit::ALBEDO);
        shader->uniform1i("texB", TextureUnit::NORMAL);

        drawQuad();
    }

    void DeferredRenderer::directLighting(const Scene& scene) {
        LOG("Direct lighting");
        nvtxRangePushA("Direct");
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
        glDepthFunc(GL_LEQUAL);

        setShader(DDIRECT);
        setCamera(*scene.getMainCamera());

        gBuffer->getColorTexture(0).bind(TextureUnit::ALBEDO);
        shader->uniform1i("albedoMap", TextureUnit::ALBEDO);
        gBuffer->getColorTexture(1).bind(TextureUnit::NORMAL);
        shader->uniform1i("normalMap", TextureUnit::NORMAL);
        gBuffer->getColorTexture(2).bind(TextureUnit::POSITION);
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

        glDisable(GL_BLEND);
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
        shader->uniformMatrix4f("modelMatrix", modelMatrix);

        glBindVertexArray(mesh->handle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, 0);
        nvtxRangePop();
    }

    void DeferredRenderer::renderSky(const Scene& scene) {
        LOG("Sky rendering");
        Transform* transform = scene.getMainCamera()->getComponent<Transform>();

        Matrix4f yawMatrix;
        yawMatrix.rotate(transform->rotation.y, 0, 1, 0);

        Matrix4f pitchMatrix;
        pitchMatrix.rotate(transform->rotation.x, 1, 0, 0);

        Matrix4f cameraBasis;
        cameraBasis[10] = -1;
        cameraBasis = yawMatrix * pitchMatrix * cameraBasis;

        if (scene.skybox) {
            setShader(SKYBOX);
            scene.skybox->bind(TextureUnit::TEXTURE);
            shader->uniform1i("skybox", TextureUnit::TEXTURE);
        }
        else if (scene.skySphere) {
            setShader(SKYSPHERE);
            scene.skySphere->bind(TextureUnit::TEXTURE);
            shader->uniform1i("tex", TextureUnit::TEXTURE);
        }
        else {
            return;
        }

        shader->uniform2f("persp", 1.0f / projMatrix.toArray()[0], 1.0f / projMatrix.toArray()[5]);
        shader->uniformMatrix4f("cameraBasis", cameraBasis);

        glDepthFunc(GL_LEQUAL);
        drawQuad();
        glDepthFunc(GL_LESS);
    }

    void DeferredRenderer::applyPostprocess() {
        LOG("Post-processing");
        nvtxRangePushA("Post-process");
        nvtxRangePushA("Bloom");
        setShader(BLOOM);
        hdrBuffer->getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        shader->uniform1f("threshold", 0);
        switchHdrBuffers();
        drawQuad();
        nvtxRangePop();

        nvtxRangePushA("Blur");
        setShader(BLUR);
        shader->uniform2f("windowSize", windowSize.width, windowSize.height);
        for (int j = 1; j < 3; j++) {
            for (int i = 0; i < 2; i++) {
                getCurrentHdrFramebuffer().getColorTexture(0).bind(TextureUnit::TEXTURE);
                glGenerateMipmap(GL_TEXTURE_2D);
                shader->uniform1i("tex", TextureUnit::TEXTURE);
                shader->uniform2f("direction", i == 0 ? j : 0, i == 0 ? 0 : j);
                switchHdrBuffers();
                drawQuad();
            }
        }
        nvtxRangePop();

        nvtxRangePushA("Tonemap");
        setShader(TONEMAP);
        hdrBuffer->getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        getCurrentHdrFramebuffer().getColorTexture(0).bind(TextureUnit::BLOOM);
        shader->uniform1i("bloomTex", TextureUnit::BLOOM);
        switchBuffers();
        drawQuad();
        nvtxRangePop();

        nvtxRangePushA("Gamma");
        setShader(GAMMA);
        getCurrentFramebuffer().getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        switchBuffers();
        drawQuad();
        nvtxRangePop();

        nvtxRangePushA("FXAA");
        setShader(FXAA);
        getCurrentFramebuffer().getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        glGenerateMipmap(GL_TEXTURE_2D);
        shader->uniform2f("rcpScreenSize", 1.0f / windowSize.width, 1.0f / windowSize.height);
        switchBuffers();
        drawQuad();
        nvtxRangePop();
        nvtxRangePop();
    }

    void DeferredRenderer::renderShadowMaps(const Scene& scene) {
        setShader(SHADOW);

        glViewport(0, 0, 4096, 4096);
        shadowBuffer->bind();
        glClear(GL_DEPTH_BUFFER_BIT);

        for (Entity* entity : scene.lights) {
            Transform* t = entity->getComponent<Transform>();
            Camera* camera = entity->getComponent<Camera>();
            DirectionalLight* light = entity->getComponent<DirectionalLight>();

            setCamera(*entity);

            Matrix4f shadowSpace;
            shadowSpace = viewMatrix * shadowSpace;
            shadowSpace = projMatrix * shadowSpace;
            light->shadowSpace = shadowSpace;

            shadowBuffer->addDepthTexture(light->shadowMap);
            shadowBuffer->validate();

            renderScene(scene);
        }
    }

    void DeferredRenderer::renderFramebuffer(const Framebuffer& framebuffer) {
        LOG("Rendering framebuffer");
        setShader(TEXTURE);
        framebuffer.getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        drawQuad();
    }
}

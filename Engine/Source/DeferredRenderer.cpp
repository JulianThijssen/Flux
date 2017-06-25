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

#define DEBUG_MODE

#ifdef DEBUG_MODE
#define LOG(str) do { Log::debug(str); } while(false)
#else
#define LOG(str) do { } while(false)
#endif

namespace Flux {
    bool DeferredRenderer::create(const Scene& scene) {
        shaders[IBL] = Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/IBL.frag");
        shaders[DIRECT] = Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/Lighting.frag");
        shaders[SKYBOX] = Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Skybox.frag");
        shaders[TEXTURE] = Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Texture.frag");
        shaders[FXAA] = Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/FXAAQuality.frag");
        shaders[GAMMA] = Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/GammaCorrection.frag");
        shaders[TONEMAP] = Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Tonemap.frag");
        shaders[SKYSPHERE] = Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Skysphere.frag");
        shaders[BLOOM] = Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Bloom.frag");
        shaders[BLUR] = Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Blur.frag");
        shaders[SSAO] = Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/SSAO.frag");
        shaders[GBUFFER] = Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/GBuffer.frag");
        shaders[DINDIRECT] = Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/DeferredIndirect.frag");
        shaders[DDIRECT] = Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/DeferredDirect.frag");

        for (auto kv : shaders) {
            if (kv.second == nullptr) {
                return false;
            }
        }

        if (scene.skybox) {
            iblSceneInfo.PrecomputeEnvironmentData(*scene.skybox);
        }
        else if (scene.skySphere) {
            iblSceneInfo.PrecomputeEnvironmentData(*scene.skySphere);
        }

        ssaoInfo.generate(30, 16);

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

        gBuffer = new Framebuffer(width, height);
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

    void DeferredRenderer::onResize(unsigned int width, unsigned int height) {
        createGBuffer(width, height);

        hdrBuffer = new Framebuffer(width, height);
        hdrBuffer->bind();
        hdrBuffer->addColorTexture(0, TextureLoader::createEmpty(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT, Sampling::NEAREST, true));
        hdrBuffer->addDepthTexture(gBufferInfo.depthTex);
        hdrBuffer->validate();
        hdrBuffer->release();

        for (int i = 0; i < 2; i++) {
            Framebuffer framebuffer(width, height);
            framebuffer.bind();
            framebuffer.addColorTexture(0, TextureLoader::createEmpty(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT, Sampling::NEAREST, true));
            framebuffer.validate();
            framebuffer.release();
            hdrBackBuffers.push_back(framebuffer);
        }
        for (int i = 0; i < 2; i++) {
            Framebuffer framebuffer(width, height);
            framebuffer.bind();
            framebuffer.addColorTexture(0, TextureLoader::createEmpty(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, Sampling::LINEAR, true));
            framebuffer.validate();
            framebuffer.release();
            backBuffers.push_back(framebuffer);
        }

        setClearColor(1.0, 0.0, 1.0, 1.0);
        glViewport(0, 0, width, height);
    }

    void DeferredRenderer::update(const Scene& scene) {
        LOG("Updating");
        if (scene.getMainCamera() == nullptr)
            return;

        LOG("Rendering GBuffer");
        gBuffer->bind();
        shader = shaders[GBUFFER];
        shader->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        setCamera(*scene.getMainCamera());
        nvtxRangePushA("GBuffer");
        renderScene(scene);
        nvtxRangePop();
        LOG("Finished GBuffer");

        hdrBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT);
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
        LOG("Indirect lighting");
        nvtxRangePushA("Indirect");
        shader = shaders[DINDIRECT];
        shader->bind();

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
        nvtxRangePop();
    }

    void DeferredRenderer::directLighting(const Scene& scene) {
        LOG("Direct lighting");
        nvtxRangePushA("Direct");
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
        glDepthFunc(GL_LEQUAL);

        shader = shaders[DDIRECT];
        shader->bind();

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
            shader = shaders[SKYBOX];
            shader->bind();
            scene.skybox->bind(TextureUnit::TEXTURE);
            shader->uniform1i("skybox", TextureUnit::TEXTURE);
        }
        else if (scene.skySphere) {
            shader = shaders[SKYSPHERE];
            shader->bind();
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
        shader = shaders[BLOOM];
        shader->bind();
        hdrBuffer->getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        shader->uniform1f("threshold", 0);
        switchHdrBuffers();
        drawQuad();
        nvtxRangePop();
        nvtxRangePushA("Blur");
        shader = shaders[BLUR];
        shader->bind();
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
        shader = shaders[TONEMAP];
        shader->bind();
        hdrBuffer->getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        getCurrentHdrFramebuffer().getColorTexture(0).bind(TextureUnit::BLOOM);
        shader->uniform1i("bloomTex", TextureUnit::BLOOM);
        switchBuffers();
        drawQuad();
        nvtxRangePop();
        nvtxRangePushA("Gamma");
        shader = shaders[GAMMA];
        shader->bind();
        getCurrentFramebuffer().getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        switchBuffers();
        drawQuad();
        nvtxRangePop();
        nvtxRangePushA("FXAA");
        shader = shaders[FXAA];
        shader->bind();
        getCurrentFramebuffer().getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        glGenerateMipmap(GL_TEXTURE_2D);
        shader->uniform2f("rcpScreenSize", 1.0f / 1920, 1.0f / 1080);
        switchBuffers();
        drawQuad();
        nvtxRangePop();
        nvtxRangePop();
    }

    void DeferredRenderer::renderFramebuffer(const Framebuffer& framebuffer) {
        LOG("Rendering framebuffer");
        shader = shaders[TEXTURE];
        shader->bind();
        framebuffer.getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        drawQuad();
    }
}

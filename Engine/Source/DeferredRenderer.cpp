#include "DeferredRenderer.h"

#include <glad/glad.h>

#include "Renderer/AveragePass.h"
#include "Renderer/MultiplyPass.h"
#include "Renderer/SSAOPass.h"
#include "Renderer/SkyPass.h"
#include "Renderer/BloomPass.h"
#include "Renderer/GaussianBlurPass.h"
#include "Renderer/TonemapPass.h"
#include "Renderer/IndirectLightPass.h"
#include "Renderer/DirectLightPass.h"
#include "Renderer/GammaCorrectionPass.h"
#include "Renderer/FxaaPass.h"
#include "Renderer/ColorGradingPass.h"
#include "Renderer/FogPass.h"

#include "Transform.h"
#include "Camera.h"
#include "AttachedTo.h"
#include "MeshRenderer.h"
#include "AssetManager.h"
#include "TextureLoader.h"
#include "TextureUnit.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "Util/Path.h"
#include "Util/Size.h"

#include <iostream>

#include "Util/Matrix4f.h"
#include "nvToolsExt.h"

//#define DEBUG_MODE

#ifdef DEBUG_MODE
#define LOG(str) do { Log::debug(str); } while(false)
#else
#define LOG(str) do { } while(false)
#endif

namespace Flux {
    bool DeferredRenderer::create(const Scene& scene, const Size windowSize) {
        gBufferShader.loadFromFile("res/Shaders/Model.vert", "res/Shaders/GBuffer.frag");
        shadowShader.loadFromFile("res/Shaders/Model.vert", "res/Shaders/Shadow.frag");
        textureShader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/Texture.frag");

        createShadowMaps(scene);

        ssaoInfo.generate();

        averagePass = new AveragePass();
        multiplyPass = new MultiplyPass();
        ssaoPass = new SSAOPass();
        skyPass = new SkyPass();
        bloomPass = new BloomPass();
        gaussianBlurPass = new GaussianBlurPass();
        tonemapPass = new TonemapPass();
        indirectLightPass = new IndirectLightPass(scene);
        directLightPass = new DirectLightPass();
        gammaCorrectionPass = new GammaCorrectionPass();
        fxaaPass = new FxaaPass();
        colorGradingPass = new ColorGradingPass();
        fogPass = new FogPass();

        enable(DEPTH_TEST);
        enable(FACE_CULLING);

        return true;
    }

    void DeferredRenderer::createBackBuffers(const unsigned int width, const unsigned int height) {
        renderState.hdrBuffer.create();
        renderState.hdrBuffer.bind();
        renderState.hdrBuffer.addColorTexture(0, TextureLoader::create(windowSize.width, windowSize.height, GL_RGBA16F, GL_RGBA, GL_FLOAT, CLAMP));
        renderState.hdrBuffer.addDepthTexture(gBuffer.depthTex);
        renderState.hdrBuffer.validate();
        renderState.hdrBuffer.release();

        renderState.ldrBuffer.create();
        renderState.ldrBuffer.bind();
        renderState.ldrBuffer.addColorTexture(0, TextureLoader::create(windowSize.width, windowSize.height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, CLAMP));
        renderState.ldrBuffer.validate();
        renderState.ldrBuffer.release();

        for (int i = 0; i < 2; i++) {
            Framebuffer framebuffer;
            framebuffer.bind();
            // Textures are linearly sampled for first step of gaussian bloom blur
            framebuffer.addColorTexture(0, TextureLoader::create(windowSize.width, windowSize.height, GL_RGBA16F, GL_RGBA, GL_FLOAT, CLAMP, SamplingConfig(LINEAR, LINEAR, LINEAR)));
            framebuffer.validate();
            framebuffer.release();
            hdrBackBuffers.push_back(framebuffer);
        }
        for (int i = 0; i < 2; i++) {
            Framebuffer framebuffer;
            framebuffer.bind();
            framebuffer.addColorTexture(0, TextureLoader::create(windowSize.width, windowSize.height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, CLAMP));
            framebuffer.validate();
            framebuffer.release();
            backBuffers.push_back(framebuffer);
        }
    }

    void DeferredRenderer::createShadowMaps(const Scene& scene) {
        for (Entity* entity : scene.lights) {
            Transform* t = entity->getComponent<Transform>();
            Camera* camera = entity->getComponent<Camera>();
            DirectionalLight* dirLight = entity->getComponent<DirectionalLight>();
            PointLight* pointLight = entity->getComponent<PointLight>();

            if (dirLight) {
                dirLight->shadowBuffer.create();
                dirLight->shadowBuffer.bind();
                dirLight->shadowBuffer.disableColor();
                dirLight->shadowBuffer.release();
                dirLight->shadowMap = TextureLoader::createShadowMap(4096, 4096);
            }
            if (pointLight) {
                pointLight->shadowBuffer.create();
                pointLight->shadowBuffer.bind();
                pointLight->shadowBuffer.disableColor();
                pointLight->shadowBuffer.release();
                pointLight->shadowMap = new Cubemap();
                pointLight->shadowMap->createShadowMap(512);
            }
        }
    }

    void DeferredRenderer::onResize(const Size windowSize) {
        this->windowSize.setSize(windowSize.width, windowSize.height);

        gBuffer.create(windowSize.width, windowSize.height);
        createBackBuffers(windowSize.width, windowSize.height);

        // Generate half sized framebuffers for low-resolution SSAO rendering
        ssaoInfo.createBuffers(windowSize.width, windowSize.height);

        bloomPass->Resize(windowSize);
        gaussianBlurPass->Resize(windowSize);
        fxaaPass->Resize(windowSize);
    }

    void DeferredRenderer::update(const Scene& scene) {
        renderState.setClearColor(1.0, 0.0, 1.0, 1.0);

        LOG("Updating");
        if (scene.getMainCamera() == nullptr)
            return;

        renderShadowMaps(scene);

        glViewport(0, 0, windowSize.width, windowSize.height);
        LOG("Rendering GBuffer");
        gBuffer.bind();
        gBufferShader.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderState.setCamera(gBufferShader, *scene.getMainCamera());
        nvtxRangePushA("GBuffer");
        renderScene(scene, gBufferShader);
        nvtxRangePop();
        LOG("Finished GBuffer");

        glDepthMask(false);

        globalIllumination(scene);
        directLighting(scene);

        glDepthMask(true);
        skyPass->render(renderState, scene);
        applyPostprocess(scene);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_DEPTH_BUFFER_BIT);
        renderFramebuffer(getCurrentFramebuffer());
        LOG("Done updating");
    }

    void DeferredRenderer::globalIllumination(const Scene& scene) {
        switchHdrBuffers();

        indirectLightPass->SetGBuffer(&gBuffer);

        indirectLightPass->render(renderState, scene);

        switchBuffers();

        ssao(scene);
        multiply(scene);
    }

    void DeferredRenderer::ssao(const Scene& scene) {
        ssaoPass->SetGBuffer(&gBuffer);
        ssaoPass->SetSsaoInfo(&ssaoInfo);
        ssaoPass->SetWindowSize(&windowSize);

        ssaoPass->render(renderState, scene);
    }

    void DeferredRenderer::multiply(const Scene& scene) {
        hdrBuffer->bind();

        std::vector<Texture> v = {
            getCurrentHdrFramebuffer().getColorTexture(0),
            ssaoInfo.getCurrentBuffer()->getColorTexture(0)
        };
        multiplyPass->SetTextures(v);
        multiplyPass->render(renderState, scene);
    }

    void DeferredRenderer::directLighting(const Scene& scene) {
        directLightPass->SetGBuffer(&gBuffer);

        directLightPass->render(renderState, scene);
    }

    void DeferredRenderer::renderScene(const Scene& scene, Shader& shader) {
        for (Entity* e : scene.entities) {
            if (!e->hasComponent<Mesh>())
                continue;

            if (e->hasComponent<MeshRenderer>()) {
                MeshRenderer* mr = e->getComponent<MeshRenderer>();
                Material* material = scene.materials[mr->materialID];

                if (material) {
                    material->bind(shader);

                    renderMesh(scene, shader, e);

                    material->release(shader);
                }
            }

            //renderMesh(scene, e);
        }
    }

    void DeferredRenderer::renderMesh(const Scene& scene, Shader& shader, Entity* e) {
        nvtxRangePushA("Mesh");
        Transform* transform = e->getComponent<Transform>();
        Mesh* mesh = e->getComponent<Mesh>();

        renderState.modelMatrix.setIdentity();

        if (e->hasComponent<AttachedTo>()) {
            Entity* parent = scene.getEntityById(e->getComponent<AttachedTo>()->parentId);

            if (parent != nullptr) {
                Transform* parentT = parent->getComponent<Transform>();
                renderState.modelMatrix.translate(parentT->position);
                renderState.modelMatrix.rotate(parentT->rotation);
                renderState.modelMatrix.scale(parentT->scale);
            }
        }

        renderState.modelMatrix.translate(transform->position);
        renderState.modelMatrix.rotate(transform->rotation);
        renderState.modelMatrix.scale(transform->scale);

        Matrix4f PVM = renderState.projMatrix * renderState.viewMatrix * renderState.modelMatrix;
        shader.uniformMatrix4f("modelMatrix", renderState.modelMatrix);
        shader.uniformMatrix4f("PVM", PVM);

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
        fog(scene);
        gammaCorrection(scene);
        antiAliasing(scene);
        colorGrading(scene);

        nvtxRangePop();
    }

    void DeferredRenderer::fog(const Scene& scene) {
        fogPass->SetSource(&getCurrentFramebuffer().getColorTexture(0));
        fogPass->SetDepthMap(gBuffer.depthTex);
        switchBuffers();
        fogPass->SetTarget(&getCurrentFramebuffer());
        fogPass->SetFogColor(Vector3f(165.0f / 255, 96.0f / 255, 81.0f / 255));

        fogPass->render(renderState, scene);
    }

    void DeferredRenderer::bloom(const Scene& scene) {
        bloomPass->SetSource(&hdrBuffer->getColorTexture(0));
        bloomPass->SetTarget(&getCurrentHdrFramebuffer());

        bloomPass->render(renderState, scene);
    }

    void DeferredRenderer::blur(const Scene& scene) {
        gaussianBlurPass->SetSource(&getCurrentHdrFramebuffer().getColorTexture(0));
        gaussianBlurPass->SetTarget(&getCurrentHdrFramebuffer());

        gaussianBlurPass->render(renderState, scene);
    }

    void DeferredRenderer::tonemap(const Scene& scene) {
        tonemapPass->SetSource(&hdrBuffer->getColorTexture(0));
        tonemapPass->SetBloom(&getCurrentHdrFramebuffer().getColorTexture(0));
        tonemapPass->SetTarget(&getCurrentFramebuffer());

        tonemapPass->render(renderState, scene);
    }

    void DeferredRenderer::gammaCorrection(const Scene& scene) {
        gammaCorrectionPass->SetSource(&getCurrentFramebuffer().getColorTexture(0));
        switchBuffers();
        gammaCorrectionPass->SetTarget(&getCurrentFramebuffer());

        gammaCorrectionPass->render(renderState, scene);
    }

    void DeferredRenderer::antiAliasing(const Scene& scene) {
        fxaaPass->SetSource(&getCurrentFramebuffer().getColorTexture(0));
        switchBuffers();
        fxaaPass->SetTarget(&getCurrentFramebuffer());

        fxaaPass->render(renderState, scene);
    }

    void DeferredRenderer::colorGrading(const Scene& scene) {
        colorGradingPass->SetSource(&getCurrentFramebuffer().getColorTexture(0));
        switchBuffers();
        colorGradingPass->SetTarget(&getCurrentFramebuffer());

        colorGradingPass->render(renderState, scene);
    }

    void DeferredRenderer::renderDepth(const Scene& scene) {
        nvtxRangePushA("Depth");

        shadowShader.bind();

        glClear(GL_DEPTH_BUFFER_BIT);
        glColorMask(false, false, false, false);

        renderState.setCamera(shadowShader, *scene.getMainCamera());
        renderScene(scene, shadowShader);

        glColorMask(true, true, true, true);

        nvtxRangePop();
    }

    void DeferredRenderer::renderShadowMaps(const Scene& scene) {
        nvtxRangePushA("Shadow");

        shadowShader.bind();

        glColorMask(false, false, false, false);

        glPolygonOffset(2.5f, 10.0f);
        enable(POLYGON_OFFSET);

        for (Entity* entity : scene.lights) {
            Transform* t = entity->getComponent<Transform>();
            Camera* camera = entity->getComponent<Camera>();
            DirectionalLight* dirLight = entity->getComponent<DirectionalLight>();
            PointLight* pointLight = entity->getComponent<PointLight>();

            if (dirLight) {
                renderState.setCamera(shadowShader, *entity);

                dirLight->shadowSpace = Matrix4f::BIAS * renderState.projMatrix * renderState.viewMatrix;

                dirLight->shadowBuffer.bind();
                dirLight->shadowBuffer.addDepthTexture(dirLight->shadowMap);
                glViewport(0, 0, dirLight->shadowMap->getWidth(), dirLight->shadowMap->getHeight());

                glClear(GL_DEPTH_BUFFER_BIT);

                renderScene(scene, shadowShader);
            }
            if (pointLight) {
                Camera cam(90, 1, 0.1, 100);

                pointLight->shadowBuffer.bind();
                pointLight->shadowBuffer.disableColor();

                // Set the clear depth to be the furthest distance possible
                glClearDepth(1);

                // Set the viewport to the size of the shadow map
                glViewport(0, 0, pointLight->shadowMap->getResolution(), pointLight->shadowMap->getResolution());

                for (int i = 0; i < 6; i++) {
                    Vector3f rotation = pointLight->transforms[i];
                    t->rotation.set(rotation);

                    renderState.setCamera(shadowShader, *t, cam);

                    // Set up the framebuffer and validate it
                    pointLight->shadowBuffer.setDepthCubemap(pointLight->shadowMap, i, 0);
                    pointLight->shadowBuffer.validate();

                    // Clear the framebuffer and render the scene from the view of the light
                    glClear(GL_DEPTH_BUFFER_BIT);

                    renderScene(scene, shadowShader);
                }
            }
        }
        disable(POLYGON_OFFSET);

        glColorMask(true, true, true, true);

        nvtxRangePop();
    }

    void DeferredRenderer::renderFramebuffer(const Framebuffer& framebuffer) {
        LOG("Rendering framebuffer");
        textureShader.bind();
        framebuffer.getColorTexture(0).bind(TextureUnit::TEXTURE);
        textureShader.uniform1i("tex", TextureUnit::TEXTURE);
        renderState.drawQuad();
    }
}

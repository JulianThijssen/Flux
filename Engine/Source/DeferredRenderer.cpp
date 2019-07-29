#include "DeferredRenderer.h"

#include <glad/glad.h>

#include "Transform.h"
#include "Camera.h"
#include "AttachedTo.h"
#include "MeshRenderer.h"
#include "AssetManager.h"
#include "TextureUnit.h"
#include "TextureFactory.h"

#include "Renderer/IndirectLightPass.h"
#include "Renderer/SSAOPass.h"
#include "Renderer/DirectLightPass.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "Util/Path.h"
#include "Util/Size.h"

#include <iostream>

#include <GDT/Matrix4f.h>
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

        std::unique_ptr<TonemapPass> toneMapPass = std::make_unique<TonemapPass>();
        std::unique_ptr<IndirectLightPass> indirectLightPass = std::make_unique<IndirectLightPass>(scene);
        std::unique_ptr<SSAOPass> ssaoPass = std::make_unique<SSAOPass>();
        std::unique_ptr<DirectLightPass> directLightPass = std::make_unique<DirectLightPass>();

        indirectLightPass->SetGBuffer(&gBuffer);
        ssaoPass->SetGBuffer(&gBuffer);
        directLightPass->SetGBuffer(&gBuffer);

        addHdrPass(std::move(indirectLightPass));
        addHdrPass(std::move(ssaoPass));
        addHdrPass(std::move(directLightPass));

        setToneMapPass(std::move(toneMapPass));

        renderState.enable(FACE_CULLING);

        renderShadowMaps(scene);

        return true;
    }

    void DeferredRenderer::createBackBuffers(const unsigned int width, const unsigned int height) {
        hdrBuffer.create();
        hdrBuffer.bind();
        hdrBuffer.addColorTexture(0, createHdrTexture(width, height));
        hdrBuffer.addColorTexture(1, createHdrTexture(width, height));
        hdrBuffer.addDepthStencilTexture(gBuffer.depthTex);
        hdrBuffer.validate();
        hdrBuffer.release();

        ldrBuffer.create();
        ldrBuffer.bind();
        ldrBuffer.addColorTexture(0, createLdrTexture(width, height));
        ldrBuffer.addColorTexture(1, createLdrTexture(width, height));
        ldrBuffer.validate();
        ldrBuffer.release();
    }

    void DeferredRenderer::createShadowMaps(const Scene& scene) {
        for (Entity* entity : scene.lights) {
            Transform& t = entity->getComponent<Transform>();
            Camera& camera = entity->getComponent<Camera>();

            if (entity->hasComponent<DirectionalLight>()) {
                DirectionalLight& dirLight = entity->getComponent<DirectionalLight>();
                dirLight.shadowBuffer.create();
                dirLight.shadowBuffer.bind();
                dirLight.shadowBuffer.disableColor();
                dirLight.shadowBuffer.release();
                dirLight.shadowMap = createShadowMap(4096, 4096);
            }
            if (entity->hasComponent<PointLight>()) {
                PointLight& pointLight = entity->getComponent<PointLight>();
                pointLight.shadowBuffer.create();
                pointLight.shadowBuffer.bind();
                pointLight.shadowBuffer.disableColor();
                pointLight.shadowBuffer.release();

                pointLight.shadowMap = createShadowCubemap(512);
            }
        }
    }

    void DeferredRenderer::onResize(const Size windowSize) {
        this->windowSize.setSize(windowSize.width, windowSize.height);

        gBuffer.create(windowSize.width, windowSize.height);
        createBackBuffers(windowSize.width, windowSize.height);

        for (const std::unique_ptr<RenderPhase>& renderPass : getHdrPasses()) {
            renderPass->Resize(windowSize);
        }
        for (const std::unique_ptr<RenderPhase>& renderPass : getLdrPasses()) {
            renderPass->Resize(windowSize);
        }
    }

    void DeferredRenderer::update(const Scene& scene) {
        GLenum error = glGetError();
        if (error != 0) {
            std::cout << "ERROR: " << error << std::endl;
        }

        renderState.setClearColor(1.0, 0.0, 1.0, 1.0);

        if (scene.getMainCamera() == nullptr)
            return;

        renderShadowMaps(scene);

        renderGBuffer(scene);

        // HDR Rendering
        hdrBuffer.bind();

        for (const auto& renderPass : getHdrPasses()) {
            renderPass->SetSource(&hdrBuffer.getTexture());
            hdrBuffer.setDrawBuffer(1 - hdrBuffer.getDrawBuffer());

            renderPass->render(renderState, scene);
        }

        // Tonemap Pass
        ldrBuffer.bind();
        getToneMapPass().SetSource(&hdrBuffer.getTexture());
        getToneMapPass().render(renderState, scene);

        // LDR Rendering
        for (const auto& renderPass : getLdrPasses()) {
            renderPass->SetSource(&ldrBuffer.getTexture());
            ldrBuffer.setDrawBuffer(1 - ldrBuffer.getDrawBuffer());

            renderPass->render(renderState, scene);
        }

        renderFramebuffer(ldrBuffer);
    }

    void DeferredRenderer::renderScene(const Scene& scene, Shader& shader) {
        for (Entity* e : scene.entities) {
            if (!e->hasComponent<Mesh>())
                continue;

            if (e->hasComponent<MeshRenderer>()) {
                MeshRenderer& mr = e->getComponent<MeshRenderer>();
                Material* material = scene.materials[mr.materialID];

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
        Transform& transform = e->getComponent<Transform>();
        Mesh& mesh = e->getComponent<Mesh>();

        renderState.modelMatrix.setIdentity();
        
        if (e->hasComponent<AttachedTo>()) {
            Entity* parent = scene.getEntityById(e->getComponent<AttachedTo>().parentId);

            if (parent != nullptr) {
                Transform& parentT = parent->getComponent<Transform>();
                renderState.modelMatrix.translate(parentT.position);
                renderState.modelMatrix.rotate(parentT.rotation);
                renderState.modelMatrix.scale(parentT.scale);
            }
        }

        renderState.modelMatrix.translate(transform.position);
        renderState.modelMatrix.rotate(transform.rotation);
        renderState.modelMatrix.scale(transform.scale);

        Matrix4f PVM = renderState.projMatrix * renderState.viewMatrix * renderState.modelMatrix;
        shader.uniformMatrix4f("modelMatrix", renderState.modelMatrix);
        shader.uniformMatrix4f("PVM", PVM);

        glBindVertexArray(mesh.handle);
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indices.size(), GL_UNSIGNED_INT, 0);
        nvtxRangePop();
    }

    void DeferredRenderer::renderGBuffer(const Scene& scene)
    {
        renderState.enable(STENCIL_TEST);
        renderState.enable(DEPTH_TEST);

        nvtxRangePushA("GBuffer");

        glViewport(0, 0, windowSize.width, windowSize.height);

        LOG("Rendering GBuffer");
        gBuffer.bind();
        gBufferShader.bind();
        
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        renderState.setCamera(gBufferShader, *scene.getMainCamera());
        
        renderScene(scene, gBufferShader);
        LOG("Finished GBuffer");
        glStencilMask(0x00);
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glDepthMask(GL_FALSE);

        nvtxRangePop();
    }
    
    void DeferredRenderer::renderDepth(const Scene& scene) {
        renderState.enable(DEPTH_TEST);

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
        renderState.enable(POLYGON_OFFSET);

        renderState.enable(DEPTH_TEST);
        glDepthMask(GL_TRUE);

        for (Entity* entity : scene.lights) {
            Transform& t = entity->getComponent<Transform>();
            Camera& camera = entity->getComponent<Camera>();

            if (entity->hasComponent<DirectionalLight>()) {
                DirectionalLight& dirLight = entity->getComponent<DirectionalLight>();
                renderState.setCamera(shadowShader, *entity);

                dirLight.shadowSpace = Matrix4f::BIAS * renderState.projMatrix * renderState.viewMatrix;

                dirLight.shadowBuffer.bind();
                dirLight.shadowBuffer.addDepthTexture(dirLight.shadowMap);
                glViewport(0, 0, dirLight.shadowMap.getWidth(), dirLight.shadowMap.getHeight());

                glClear(GL_DEPTH_BUFFER_BIT);

                renderScene(scene, shadowShader);
            }
            if (entity->hasComponent<PointLight>()) {
                PointLight& pointLight = entity->getComponent<PointLight>();
                Camera cam(90, 1, 0.1, 100);

                pointLight.shadowBuffer.bind();
                pointLight.shadowBuffer.disableColor();

                // Set the clear depth to be the furthest distance possible
                glClearDepth(1);

                // Set the viewport to the size of the shadow map
                glViewport(0, 0, pointLight.shadowMap.getResolution(), pointLight.shadowMap.getResolution());

                for (int i = 0; i < 6; i++) {
                    Vector3f rotation = pointLight.transforms[i];
                    t.rotation.set(rotation);

                    renderState.setCamera(shadowShader, t, cam);

                    // Set up the framebuffer and validate it
                    pointLight.shadowBuffer.setDepthCubemap(pointLight.shadowMap, i, 0);
                    pointLight.shadowBuffer.validate();

                    // Clear the framebuffer and render the scene from the view of the light
                    glClear(GL_DEPTH_BUFFER_BIT);

                    renderScene(scene, shadowShader);
                }
            }
        }
        renderState.disable(POLYGON_OFFSET);

        glColorMask(true, true, true, true);

        nvtxRangePop();
    }

    void DeferredRenderer::renderFramebuffer(const Framebuffer& framebuffer) {
        LOG("Rendering framebuffer");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);
        glClearColor(0.5, 1, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        textureShader.bind();
        framebuffer.getTexture().bind(TextureUnit::TEXTURE);
        textureShader.uniform1i("tex", TextureUnit::TEXTURE);
        renderState.drawQuad();
    }
}

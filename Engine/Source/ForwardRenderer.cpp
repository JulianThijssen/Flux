#include "ForwardRenderer.h"

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

namespace Flux {
    bool ForwardRenderer::create(const Scene& scene) {
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
        addShader(SSAO,      Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/SSAO.frag"));

        if (!validateShaders()) {
            return false;
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

    void ForwardRenderer::onResize(unsigned int width, unsigned int height) {
        hdrBuffer = new Framebuffer(width, height);
        hdrBuffer->bind();
        hdrBuffer->addColorTexture(0, TextureLoader::createEmpty(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT, Sampling::NEAREST, true));
        hdrBuffer->addDepthTexture();
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

    void ForwardRenderer::update(const Scene& scene) {
        if (scene.getMainCamera() == nullptr)
            return;

        hdrBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        globalIllumination(scene);
        //directLighting(scene);
        renderSky(scene);
        applyPostprocess();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderFramebuffer(getCurrentFramebuffer());
    }

    void ForwardRenderer::globalIllumination(const Scene& scene) {
        setShader(IBL);

        setCamera(*scene.getMainCamera());

        iblSceneInfo.irradianceMap->bind(TextureUnit::IRRADIANCE);
        shader->uniform1i("irradianceMap", TextureUnit::IRRADIANCE);

        iblSceneInfo.prefilterEnvmap->bind(TextureUnit::PREFILTER);
        shader->uniform1i("prefilterEnvmap", TextureUnit::PREFILTER);

        iblSceneInfo.scaleBiasTexture->bind(TextureUnit::SCALEBIAS);
        shader->uniform1i("scaleBiasMap", TextureUnit::SCALEBIAS);
        //shader = shaders[SSAO];
        //shader->bind();

        //setCamera(*scene.getMainCamera());

        renderScene(scene);
    }

    void ForwardRenderer::directLighting(const Scene& scene) {
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
        glDepthFunc(GL_LEQUAL);

        setShader(DIRECT);

        setCamera(*scene.getMainCamera());

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

            renderScene(scene);
        }

        glDisable(GL_BLEND);
    }

    void ForwardRenderer::renderScene(const Scene& scene) {
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

    void ForwardRenderer::renderMesh(const Scene& scene, Entity* e) {
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
        glDrawElements(GL_TRIANGLES, (GLsizei) mesh->indices.size(), GL_UNSIGNED_INT, 0);
    }

    void ForwardRenderer::renderSky(const Scene& scene) {
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

    void ForwardRenderer::applyPostprocess() {
        setShader(BLOOM);
        hdrBuffer->getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        shader->uniform1f("threshold", 0);
        switchHdrBuffers();
        drawQuad();
        
        setShader(BLUR);
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

        setShader(TONEMAP);
        hdrBuffer->getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        getCurrentHdrFramebuffer().getColorTexture(0).bind(TextureUnit::BLOOM);
        shader->uniform1i("bloomTex", TextureUnit::BLOOM);
        switchBuffers();
        drawQuad();

        setShader(GAMMA);
        getCurrentFramebuffer().getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        switchBuffers();
        drawQuad();

        setShader(FXAA);
        getCurrentFramebuffer().getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        glGenerateMipmap(GL_TEXTURE_2D);
        shader->uniform2f("rcpScreenSize", 1.0f / windowSize.width, 1.0f / windowSize.height);
        switchBuffers();
        drawQuad();
    }

    void ForwardRenderer::renderFramebuffer(const Framebuffer& framebuffer) {
        setShader(TEXTURE);
        framebuffer.getColorTexture(0).bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        drawQuad();
    }
}

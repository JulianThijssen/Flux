#include "ForwardRenderer.h"

#include <glad/glad.h>

#include "ShaderLoader.h"

#include "Transform.h"
#include "Camera.h"
#include "AttachedTo.h"
#include "MeshRenderer.h"
#include "AssetManager.h"

#include "Exceptions/ShaderCompilationException.h"
#include "Exceptions/ShaderLinkException.h"

#include <iostream>

namespace Flux {
    bool ForwardRenderer::create() {
        try {
            shader = ShaderLoader::loadShaderProgram("res/IBL.vert", "res/IBL.frag");
        }
        catch (const ShaderCompilationException& e) {
            Log::error(e.what());
            return false;
        }
        catch (const ShaderLinkException& e) {
            Log::error(e.what());
            return false;
        }
        
        const char* paths[] = {
            "res/Materials/Grace_RIGHT.png",
            "res/Materials/Grace_LEFT.png",
            "res/Materials/Grace_TOP.png",
            "res/Materials/Grace_BOTTOM.png",
            "res/Materials/Grace_FRONT.png",
            "res/Materials/Grace_BACK.png",
        };
        cubemap.create(paths, false);

        irradianceMap = new IrradianceMap(cubemap);
        irradianceMap->generate(32);

        prefilterEnvmap = new PrefilterEnvmap(cubemap);
        prefilterEnvmap->generate();

        scaleBiasTexture = new ScaleBiasTexture();
        scaleBiasTexture->generate();

        setClearColor(1.0, 0.0, 1.0, 1.0);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        return true;
    }

    void ForwardRenderer::update(const Scene& scene) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (scene.getMainCamera() == nullptr)
            return;

        glViewport(0, 0, 1024, 768);
        shader->bind();

        Transform* ct = scene.getMainCamera()->getComponent<Transform>();
        shader->uniform3f("camPos", ct->position.x, ct->position.y, ct->position.z);
        scene.getMainCamera()->getComponent<Camera>()->loadProjectionMatrix(projMatrix);
        viewMatrix.setIdentity();
        viewMatrix.translate(-ct->position);
        viewMatrix.rotate(-ct->rotation);

        shader->uniformMatrix4f("projMatrix", projMatrix);
        shader->uniformMatrix4f("viewMatrix", viewMatrix);

        for (Entity* light : scene.lights) {
            PointLight* point = light->getComponent<PointLight>();
            Transform* transform = light->getComponent<Transform>();

            shader->uniform3f("pointLight.position", transform->position.x, transform->position.y, transform->position.z);

            renderScene(scene);
        }

        //shader->release();
    }

    void ForwardRenderer::renderScene(const Scene& scene) {
        for (Entity* e : scene.entities) {
            if (!e->hasComponent<Mesh>())
                continue;

            shader->uniform1i("material.hasDiffuseMap", 0);
            shader->uniform1i("material.hasNormalMap", 0);
            shader->uniform1i("material.hasMetalMap", 0);
            shader->uniform1i("material.hasRoughnessMap", 0);
            if (e->hasComponent<MeshRenderer>()) {
                MeshRenderer* mr = e->getComponent<MeshRenderer>();
                Material* material = scene.materials[mr->materialID];

                if (material) {
                    if (material->diffuseTex) {
                        glActiveTexture(GL_TEXTURE0);
                        material->diffuseTex->bind();
                        shader->uniform1i("material.diffuseMap", 0);
                        shader->uniform1i("material.hasDiffuseMap", 1);
                    }
                    if (material->normalTex) {
                        glActiveTexture(GL_TEXTURE3);
                        material->normalTex->bind();
                        shader->uniform1i("material.normalMap", 3);
                        shader->uniform1i("material.hasNormalMap", 1);
                    }
                    if (material->metalTex) {
                        glActiveTexture(GL_TEXTURE4);
                        material->metalTex->bind();
                        shader->uniform1i("material.metalMap", 4);
                        shader->uniform1i("material.hasMetalMap", 1);
                    }
                    if (material->roughnessTex) {
                        glActiveTexture(GL_TEXTURE5);
                        material->roughnessTex->bind();
                        shader->uniform1i("material.roughnessMap", 5);
                        shader->uniform1i("material.hasRoughnessMap", 1);
                    }
                }
            }

            glActiveTexture(GL_TEXTURE1);
            cubemap.bind();
            shader->uniform1i("cubemap", 1);

            glActiveTexture(GL_TEXTURE2);
            irradianceMap->bind();
            shader->uniform1i("irradianceMap", 2);

            glActiveTexture(GL_TEXTURE6);
            prefilterEnvmap->bind();
            shader->uniform1i("prefilterEnvmap", 6);

            glActiveTexture(GL_TEXTURE7);
            scaleBiasTexture->bind();
            shader->uniform1i("scaleBiasMap", 7);

            renderMesh(scene, e);

            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void ForwardRenderer::renderMesh(const Scene& scene, Entity* e) {
        Transform* transform = e->getComponent<Transform>();
        Mesh* mesh = e->getComponent<Mesh>();
        glBindVertexArray(mesh->handle);

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
        transform->rotation.x += 0.01f;
        modelMatrix.scale(transform->scale);
        shader->uniformMatrix4f("modelMatrix", modelMatrix);

        glDrawArrays(GL_TRIANGLES, 0, (GLsizei) mesh->indices.size());
    }
}

#include "ForwardRenderer.h"

#include <glad/glad.h>

#include "Transform.h"
#include "Camera.h"
#include "AttachedTo.h"
#include "MeshRenderer.h"
#include "AssetManager.h"

#include "DirectionalLight.h"
#include "PointLight.h"

#include <iostream>

namespace Flux {
    bool ForwardRenderer::create() {
        IBLShader = Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/IBL.frag");
        lightShader = Shader::fromFile("res/Shaders/Model.vert", "res/Shaders/Lighting.frag");
        skyboxShader = Shader::fromFile("res/Shaders/Skybox.vert", "res/Shaders/Skybox.frag");
        if (IBLShader == nullptr || skyboxShader == nullptr || lightShader == nullptr) {
            return false;
        }

        const char* paths[] = {
            "res/Materials/Grace_RIGHT.png",
            "res/Materials/Grace_LEFT.png",
            "res/Materials/Grace_TOP.png",
            "res/Materials/Grace_BOTTOM.png",
            "res/Materials/Grace_FRONT.png",
            "res/Materials/Grace_BACK.png"
        };

        skybox = new Skybox(paths);

        iblSceneInfo.PrecomputeEnvironmentData(*skybox);

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

        shader = IBLShader;
        shader->bind();

        setCamera(*scene.getMainCamera());

        renderScene(scene);

        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ZERO);
        glDepthFunc(GL_LEQUAL);

        shader = lightShader;
        shader->bind();

        setCamera(*scene.getMainCamera());

        for (Entity* light : scene.lights) {
            DirectionalLight* directionalLight = light->getComponent<DirectionalLight>();
            PointLight* pointLight = light->getComponent<PointLight>();
            Transform* transform = light->getComponent<Transform>();

            if (directionalLight) {
                shader->uniform3f("dirLight.direction", directionalLight->direction);
                shader->uniform1i("isDirLight", true);
                shader->uniform1i("isPointLight", false);
            }
            else if (pointLight) {
                shader->uniform3f("pointLight.position", transform->position);
                shader->uniform1i("isPointLight", true);
                shader->uniform1i("isDirLight", false);
            }
            else {
                continue;
            }

            renderScene(scene);
        }

        glDisable(GL_BLEND);

        renderSkybox(scene);
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
                    uploadMaterial(*material);
                }
            }
            
            iblSceneInfo.irradianceMap->bind(TEX_UNIT_IRRADIANCE);
            shader->uniform1i("irradianceMap", TEX_UNIT_IRRADIANCE);

            iblSceneInfo.prefilterEnvmap->bind(TEX_UNIT_PREFILTER);
            shader->uniform1i("prefilterEnvmap", TEX_UNIT_PREFILTER);

            iblSceneInfo.scaleBiasTexture->bind(TEX_UNIT_SCALEBIAS);
            shader->uniform1i("scaleBiasMap", TEX_UNIT_SCALEBIAS);

            renderMesh(scene, e);

            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void ForwardRenderer::uploadMaterial(const Material& material) {
        if (material.diffuseTex) {
            material.diffuseTex->bind(TEX_UNIT_DIFFUSE);
            shader->uniform1i("material.diffuseMap", TEX_UNIT_DIFFUSE);
            shader->uniform1i("material.hasDiffuseMap", 1);
        }
        if (material.normalTex) {
            material.normalTex->bind(TEX_UNIT_NORMAL);
            shader->uniform1i("material.normalMap", TEX_UNIT_NORMAL);
            shader->uniform1i("material.hasNormalMap", 1);
        }
        if (material.metalTex) {
            material.metalTex->bind(TEX_UNIT_METALNESS);
            shader->uniform1i("material.metalMap", TEX_UNIT_METALNESS);
            shader->uniform1i("material.hasMetalMap", 1);
        }
        if (material.roughnessTex) {
            material.roughnessTex->bind(TEX_UNIT_ROUGHNESS);
            shader->uniform1i("material.roughnessMap", TEX_UNIT_ROUGHNESS);
            shader->uniform1i("material.hasRoughnessMap", 1);
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

    void ForwardRenderer::renderSkybox(const Scene& scene) {
        shader = skyboxShader;
        shader->bind();

        shader->uniformMatrix4f("projMatrix", projMatrix);
        shader->uniformMatrix4f("viewMatrix", viewMatrix);
        modelMatrix.setIdentity();
        modelMatrix.scale(Vector3f(20000, 20000, 20000));
        shader->uniformMatrix4f("modelMatrix", modelMatrix);

        skybox->bind(TEX_UNIT_DIFFUSE);
        shader->uniform1i("skybox", 0);

        skybox->render();
    }
}

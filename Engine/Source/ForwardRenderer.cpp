#include "ForwardRenderer.h"

#include <glad/glad.h>

#include "Transform.h"
#include "Camera.h"
#include "AttachedTo.h"
#include "MeshRenderer.h"
#include "AssetManager.h"

#include <iostream>

namespace Flux {
    bool ForwardRenderer::create() {
        lightShader = Shader::fromFile("res/IBL.vert", "res/IBL.frag");
        skyboxShader = Shader::fromFile("res/skybox.vert", "res/skybox.frag");
        if (lightShader == nullptr || skyboxShader == nullptr) {
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
        shader = lightShader;
        shader->bind();

        Transform* ct = scene.getMainCamera()->getComponent<Transform>();
        shader->uniform3f("camPos", ct->position);
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
            
            iblSceneInfo.irradianceMap->bind(IRRADIANCE_UNIT);
            shader->uniform1i("irradianceMap", IRRADIANCE_UNIT);

            iblSceneInfo.prefilterEnvmap->bind(PREFILTER_UNIT);
            shader->uniform1i("prefilterEnvmap", PREFILTER_UNIT);

            iblSceneInfo.scaleBiasTexture->bind(SCALEBIAS_UNIT);
            shader->uniform1i("scaleBiasMap", SCALEBIAS_UNIT);

            renderMesh(scene, e);

            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void ForwardRenderer::uploadMaterial(const Material& material) {
        if (material.diffuseTex) {
            material.diffuseTex->bind(DIFFUSE_UNIT);
            shader->uniform1i("material.diffuseMap", DIFFUSE_UNIT);
            shader->uniform1i("material.hasDiffuseMap", 1);
        }
        if (material.normalTex) {
            material.normalTex->bind(NORMAL_UNIT);
            shader->uniform1i("material.normalMap", NORMAL_UNIT);
            shader->uniform1i("material.hasNormalMap", 1);
        }
        if (material.metalTex) {
            material.metalTex->bind(METALNESS_UNIT);
            shader->uniform1i("material.metalMap", METALNESS_UNIT);
            shader->uniform1i("material.hasMetalMap", 1);
        }
        if (material.roughnessTex) {
            material.roughnessTex->bind(ROUGHNESS_UNIT);
            shader->uniform1i("material.roughnessMap", ROUGHNESS_UNIT);
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
        transform->rotation.x += 0.01f;
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

        skybox->bind(DIFFUSE_UNIT);
        shader->uniform1i("skybox", 0);

        skybox->render();
    }
}

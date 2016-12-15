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
            shader = ShaderLoader::loadShaderProgram("res/basic.vert", "res/basic.frag");
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
        cubemap.create(paths);

        irradianceMap = new IrradianceMap(cubemap);
        irradianceMap->generate(32, 32);

        setClearColor(1.0, 0.0, 1.0, 1.0);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        return true;
    }

    void ForwardRenderer::update(const Scene& scene) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, 1024, 768);
        shader->bind();

        Transform* ct = scene.getMainCamera().getComponent<Transform>();
        shader->uniform3f("camPos", ct->position.x, ct->position.y, ct->position.z);
        scene.getMainCamera().getComponent<Camera>()->loadProjectionMatrix(projMatrix);
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

            glActiveTexture(GL_TEXTURE0);
            if (e->hasComponent<MeshRenderer>()) {
                MeshRenderer* mr = e->getComponent<MeshRenderer>();
                Material* material = AssetManager::getMaterial(mr->materialID);

                if (material) {
                    if (material->diffuseTex) {
                        material->diffuseTex->bind();
                        shader->uniform1i("material.diffuseMap", 0);
                        shader->uniform1i("material.hasDiffuseMap", 1);
                    }
                    else {
                        shader->uniform1i("material.hasDiffuseMap", 0);
                    }
                }
            }

            glActiveTexture(GL_TEXTURE1);
            cubemap.bind();
            shader->uniform1i("cubemap", 1);

            glActiveTexture(GL_TEXTURE2);
            irradianceMap->bind();
            shader->uniform1i("irradianceMap", 2);

            renderMesh(e);

            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void ForwardRenderer::renderMesh(Entity* e) {
        Transform* transform = e->getComponent<Transform>();
        Mesh* mesh = e->getComponent<Mesh>();
        glBindVertexArray(mesh->handle);

        modelMatrix.setIdentity();

        if (e->hasComponent<AttachedTo>()) {
            Entity* parent = e->getComponent<AttachedTo>()->parent;
            Transform* parentT = parent->getComponent<Transform>();
            modelMatrix.translate(parentT->position);
            modelMatrix.rotate(parentT->rotation);
            modelMatrix.scale(parentT->scale);
        }

        modelMatrix.translate(transform->position);
        transform->rotation.y += 0.01f;
        modelMatrix.rotate(transform->rotation);
        modelMatrix.scale(transform->scale);
        shader->uniformMatrix4f("modelMatrix", modelMatrix);

        glDrawArrays(GL_TRIANGLES, 0, (GLsizei) mesh->indices.size());
    }
}

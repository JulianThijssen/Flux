#include "ForwardRenderer.h"

#include <glad/glad.h>

#include "ShaderLoader.h"
#include "ModelLoader.h"
#include "TextureLoader.h"
#include "Path.h"

#include "Transform.h"
#include "Camera.h"
#include <iostream>

namespace Flux {
    void ForwardRenderer::create() {
        shader = ShaderLoader::loadShaders("res/basic.vert", "res/basic.frag");

        setClearColor(1.0, 0.0, 1.0, 1.0);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        tex = TextureLoader::loadTexture(Path("res/Test.jpg"));
    }

    void ForwardRenderer::update(const Scene& scene) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->bind();

        Transform ct = *static_cast<Transform*>(scene.getMainCamera().getComponent<Transform>());
        scene.getMainCamera().getComponent<Camera>()->loadProjectionMatrix(projMatrix);
        viewMatrix.setIdentity();
        viewMatrix.translate(-ct.position);

        shader->uniformMatrix4f("projMatrix", projMatrix);
        shader->uniformMatrix4f("viewMatrix", viewMatrix);

        tex->bind();
        shader->uniform1i("diffuseTex", 0);

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

            renderMesh(e);
        }
    }

    void ForwardRenderer::renderMesh(Entity* e) {
        Transform* transform = e->getComponent<Transform>();
        Mesh* mesh = e->getComponent<Mesh>();
        glBindVertexArray(mesh->handle);

        modelMatrix.setIdentity();
        modelMatrix.translate(transform->position);
        modelMatrix.rotate(transform->rotation);
        modelMatrix.scale(transform->scale);
        shader->uniformMatrix4f("modelMatrix", modelMatrix);

        glDrawArrays(GL_TRIANGLES, 0, mesh->numFaces * 3);
    }
}

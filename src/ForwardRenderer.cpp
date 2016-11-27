#include "ForwardRenderer.h"

#include <glad/glad.h>

#include "ShaderLoader.h"
#include "ModelLoader.h"
#include "Path.h"

#include "Transform.h"
#include "Camera.h"
#include <iostream>

namespace Flux {
    void ForwardRenderer::update(const Scene& scene) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->bind();

        Transform ct = *static_cast<Transform*>(scene.getMainCamera().getComponent<Transform>());
        scene.getMainCamera().getComponent<Camera>()->loadProjectionMatrix(projMatrix);
        viewMatrix.setIdentity();
        viewMatrix.translate(-ct.position);

        shader->uniformMatrix4f("projMatrix", projMatrix);
        shader->uniformMatrix4f("viewMatrix", viewMatrix);


        for (Entity* e : scene.entities) {
            Transform* transform = e->getComponent<Transform>();
            Model* model = e->getComponent<Model>();
            glBindVertexArray(model->meshes[0].handle);

            modelMatrix.setIdentity();
            modelMatrix.translate(transform->position);
            modelMatrix.rotate(transform->rotation);
            modelMatrix.scale(transform->scale);
            shader->uniformMatrix4f("modelMatrix", modelMatrix);

            glDrawArrays(GL_TRIANGLES, 0, model->meshes[0].numFaces * 3);
        }

        //shader->release();
    }
}

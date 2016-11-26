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

        glBindVertexArray(model.meshes[0].handle);
        shader->bind();

        Transform ct = *static_cast<Transform*>(scene.getMainCamera().getComponent<Transform>());
        scene.getMainCamera().getComponent<Camera>()->loadProjectionMatrix(projMatrix);
        viewMatrix.setIdentity();
        viewMatrix.translate(-ct.position);

        shader->uniformMatrix4f("projMatrix", projMatrix);
        shader->uniformMatrix4f("viewMatrix", viewMatrix);
        shader->uniformMatrix4f("modelMatrix", modelMatrix);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        //shader->release();
    }
}

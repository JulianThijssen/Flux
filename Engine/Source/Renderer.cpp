#include "Renderer.h"

#include <glad/glad.h>

namespace Flux {
    void Renderer::setClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    void Renderer::setCamera(Entity& camera) {
        Transform* ct = camera.getComponent<Transform>();
        Camera* cam = camera.getComponent<Camera>();

        // Set the projection matrix from the camera parameters
        camera.getComponent<Camera>()->loadProjectionMatrix(projMatrix);

        // Set the view matrix to the camera view
        viewMatrix.setIdentity();
        viewMatrix.rotate(-ct->rotation);
        viewMatrix.translate(-ct->position);

        shader->uniform3f("camPos", ct->position);
        shader->uniformMatrix4f("projMatrix", projMatrix);
        shader->uniformMatrix4f("viewMatrix", viewMatrix);
    }
}

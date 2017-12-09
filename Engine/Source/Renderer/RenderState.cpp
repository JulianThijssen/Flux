#include "Renderer/RenderState.h"

#include "Shader.h"
#include "Entity.h"
#include "Transform.h"
#include "Camera.h"

namespace Flux {
    GLuint RenderState::quadVao = 0;

    void RenderState::enable(Capability capability) {
        glEnable(capability);
    }

    void RenderState::disable(Capability capability) {
        glDisable(capability);
    }

    void RenderState::setClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    void RenderState::drawQuad() const {
        glBindVertexArray(quadVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void RenderState::setCamera(Shader& shader, Entity& camera) {
        Transform* ct = camera.getComponent<Transform>();
        Camera* cam = camera.getComponent<Camera>();

        // Set the projection matrix from the camera parameters
        camera.getComponent<Camera>()->loadProjectionMatrix(projMatrix);

        // Set the view matrix to the camera view
        viewMatrix.setIdentity();
        viewMatrix.rotate(-ct->rotation);
        viewMatrix.translate(-ct->position);

        shader.uniform3f("camPos", ct->position);
        shader.uniformMatrix4f("projMatrix", projMatrix);
        shader.uniformMatrix4f("viewMatrix", viewMatrix);
        shader.uniform1f("zNear", cam->getZNear());
        shader.uniform1f("zFar", cam->getZFar());
    }
}

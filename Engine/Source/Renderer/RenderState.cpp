#include "Renderer/RenderState.h"

#include "Framebuffer.h"
#include "Shader.h"
#include "Entity.h"
#include "Transform.h"
#include "Camera.h"

namespace Flux {
    GLuint RenderState::quadVao = 0;

    const Framebuffer* RenderState::currentFramebuffer = 0;

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

        setCamera(shader, *ct, *cam);
    }

    void RenderState::setCamera(Shader& shader, Transform& t, Camera& cam) {
        // Set the projection matrix from the camera parameters
        cam.loadProjectionMatrix(projMatrix);

        // Set the view matrix to the camera view
        viewMatrix.setIdentity();
        viewMatrix.rotate(-t.rotation);
        viewMatrix.translate(-t.position);

        shader.uniform3f("camPos", t.position);
        shader.uniformMatrix4f("projMatrix", projMatrix);
        shader.uniformMatrix4f("viewMatrix", viewMatrix);
        shader.uniform1f("zNear", cam.getZNear());
        shader.uniform1f("zFar", cam.getZFar());
    }
}

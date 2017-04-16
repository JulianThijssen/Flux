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

    void Renderer::drawQuad() {
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    const Framebuffer& Renderer::getCurrentFramebuffer() {
        return backBuffers[currentFramebuffer];
    }

    const Framebuffer& Renderer::getCurrentHdrFramebuffer() {
        return hdrBackBuffers[currentFramebuffer];
    }

    const Framebuffer& Renderer::getOtherFramebuffer() {
        unsigned int otherFramebuffer = currentFramebuffer ? 0 : 1;
        return backBuffers[otherFramebuffer];
    }

    void Renderer::switchBuffers() {
        currentFramebuffer = currentFramebuffer ? 0 : 1;
        backBuffers[currentFramebuffer].bind();
    }

    void Renderer::switchHdrBuffers() {
        currentFramebuffer = currentFramebuffer ? 0 : 1;
        hdrBackBuffers[currentFramebuffer].bind();
    }
}

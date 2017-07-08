#include "Renderer.h"

#include <glad/glad.h>

namespace Flux {
    void Renderer::addShader(const ShaderName name, Shader* shader) {
        shaders[name] = shader;
    }

    void Renderer::setShader(const ShaderName shaderName) {
        shader = shaders[shaderName];
        shader->bind();
    }

    bool Renderer::validateShaders() {
        for (auto kv : shaders) {
            if (kv.second == nullptr) {
                return false;
            }
        }
        return true;
    }

    bool Renderer::isEnabled(RenderPhase phase) {
        return renderPhases[phase];
    }

    void Renderer::enableRenderPhase(RenderPhase phase) {
        renderPhases[phase] = true;
    }

    void Renderer::disableRenderPhase(RenderPhase phase) {
        renderPhases[phase] = false;
    }

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
        shader->uniform1f("zNear", cam->getZNear());
        shader->uniform1f("zFar", cam->getZFar());
    }

    void Renderer::drawQuad() {
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    const Framebuffer& Renderer::getCurrentFramebuffer() {
        return backBuffers[currentFramebuffer];
    }

    const Framebuffer& Renderer::getCurrentHdrFramebuffer() {
        return hdrBackBuffers[currentHdrFramebuffer];
    }

    const Framebuffer& Renderer::getOtherFramebuffer() {
        unsigned int otherFramebuffer = currentFramebuffer ? 0 : 1;
        return backBuffers[otherFramebuffer];
    }

    const Framebuffer& Renderer::getOtherHdrFramebuffer() {
        unsigned int otherHdrFramebuffer = currentHdrFramebuffer ? 0 : 1;
        return hdrBackBuffers[otherHdrFramebuffer];
    }

    void Renderer::switchBuffers() {
        currentFramebuffer = currentFramebuffer ? 0 : 1;
        backBuffers[currentFramebuffer].bind();
    }

    void Renderer::switchHdrBuffers() {
        currentHdrFramebuffer = currentHdrFramebuffer ? 0 : 1;
        hdrBackBuffers[currentHdrFramebuffer].bind();
    }
}

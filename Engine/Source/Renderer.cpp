#include "Renderer.h"

#include <glad/glad.h>

namespace Flux {
    void Renderer::addRenderPhase(RenderPhase* phase) {
        renderPhases.push(phase);
    }

    void Renderer::enable(Capability capability) {
        glEnable(capability);
    }

    void Renderer::disable(Capability capability) {
        glDisable(capability);
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

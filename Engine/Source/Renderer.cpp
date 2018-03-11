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
}

#include "Renderer.h"

#include <glad/glad.h>

namespace Flux {
    void Renderer::addRenderPhase(RenderPhase* phase) {
        renderPhases.push(phase);
    }
}

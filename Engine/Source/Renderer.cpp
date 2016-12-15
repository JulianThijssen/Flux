#include "Renderer.h"

#include <glad/glad.h>

namespace Flux {
    void Renderer::setClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }
}

#include "Renderer.h"

#include <glad/glad.h>

#include "ShaderLoader.h"
#include "ModelLoader.h"
#include "Path.h"

namespace Flux {
    void Renderer::setClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }
}

#include "Renderer.h"

#include <glad/glad.h>

#include "ShaderLoader.h"
#include "ModelLoader.h"
#include "Path.h"

namespace Flux {
    void Renderer::create() {
        shader = ShaderLoader::loadShaders("res/basic.vert", "res/basic.frag");

        setClearColor(1.0, 0.0, 1.0, 1.0);

        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::setClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }
}

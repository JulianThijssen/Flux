#include "Renderer/RenderState.h"

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

    void RenderState::drawQuad() {
        if (quadVao == 0) {
            glGenVertexArrays(1, &quadVao);
        }

        glBindVertexArray(quadVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

#include "Renderer.h"

#include <glad/glad.h>

void Renderer::update() {
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);
}

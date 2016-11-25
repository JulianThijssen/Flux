#include "Renderer.h"

#include <glad/glad.h>

#include "ShaderLoader.h"

void Renderer::create() {
    shader = ShaderLoader::loadShaders("res/basic.vert", "res/basic.frag");

    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
}

void Renderer::update() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //shader->release();
}

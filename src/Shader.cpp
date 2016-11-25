#include "Shader.h"

Shader::Shader(GLuint handle) {
    this->handle = handle;
}

int Shader::getLocation(const char* uniform) {
    return glGetUniformLocation(handle, uniform);
}

void Shader::bind() {
    glUseProgram(handle);
}

void Shader::release() {
    glUseProgram(0);
}

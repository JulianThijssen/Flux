#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

class Shader {
public:
    GLuint handle;

    Shader(GLuint handle);
    int getLocation(const char* uniform);
    void bind();
    void release();
};

#endif /* SHADER_H */

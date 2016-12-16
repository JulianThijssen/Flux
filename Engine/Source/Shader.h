#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <unordered_map>
#include <string>

namespace Flux {
    class Matrix4f;

    class Shader {
    public:
        GLuint handle;

        Shader(GLuint handle);
        ~Shader();
        void bind();
        void release();
        
        void uniform1i(const char* name, int value);
        void uniform1f(const char* name, float value);
        void uniform3f(const char* name, float v0, float v1, float v2);
        void uniformMatrix4f(const char* name, Matrix4f& m);
    private:
        int location(const char* uniform);

        std::unordered_map<std::string, int> locationMap;
    };
}

#endif /* SHADER_H */

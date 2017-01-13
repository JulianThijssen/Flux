#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <unordered_map>
#include <string>

namespace Flux {
    class Vector3f;
    class Matrix4f;

    class Shader {
    public:
        Shader(GLuint handle);
        ~Shader();
        void bind();
        void release();
        
        void uniform1i(const char* name, int value);
        void uniform2i(const char* name, int v0, int v1);
        void uniform1f(const char* name, float value);
        void uniform2f(const char* name, float v0, float v1);
        void uniform3f(const char* name, float v0, float v1, float v2);
        void uniform3f(const char* name, Vector3f v);
        void uniformMatrix4f(const char* name, Matrix4f& m);

        static Shader* fromFile(std::string vertPath, std::string fragPath);
    private:
        int location(const char* uniform);

        GLuint handle;
        std::unordered_map<std::string, int> locationMap;
    };
}

#endif /* SHADER_H */

#include "Shader.h"

#include "Matrix4f.h"

#include "ShaderLoader.h"
#include "Exceptions/ShaderCompilationException.h"
#include "Exceptions/ShaderLinkException.h"
#include "Log.h"

namespace Flux {
    Shader::Shader(GLuint handle) {
        this->handle = handle;
    }

    Shader::~Shader() {
        glDeleteProgram(handle);
    }

    void Shader::bind() {
        glUseProgram(handle);
    }

    void Shader::release() {
        glUseProgram(0);
    }

    void Shader::uniform1i(const char* name, int value) {
        glUniform1i(location(name), value);
    }

    void Shader::uniform2i(const char* name, int v0, int v1) {
        glUniform2i(location(name), v0, v1);
    }

    void Shader::uniform1f(const char* name, float value) {
        glUniform1f(location(name), value);
    }

    void Shader::uniform2f(const char* name, float v0, float v1) {
        glUniform2f(location(name), v0, v1);
    }

    void Shader::uniform3f(const char* name, float v0, float v1, float v2) {
        glUniform3f(location(name), v0, v1, v2);
    }

    void Shader::uniformMatrix4f(const char* name, Matrix4f& m) {
        glUniformMatrix4fv(location(name), 1, false, m.toArray());
    }

    Shader* Shader::fromFile(std::string vertPath, std::string fragPath) {
        Shader* shader;
        try {
            shader = ShaderLoader::loadShaderProgram(vertPath, fragPath);
        }
        catch (const ShaderCompilationException& e) {
            Log::error(e.what());
        }
        catch (const ShaderLinkException& e) {
            Log::error(e.what());
        }

        return shader;
    }

    int Shader::location(const char* name) {
        std::unordered_map<std::string, int>::const_iterator it = locationMap.find(std::string(name));
        if (it != locationMap.end()) {
            return it->second;
        }
        else {
            int location = glGetUniformLocation(handle, name);
            locationMap[std::string(name)] = location;
            return location;
        }
    }
}

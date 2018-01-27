#include "Shader.h"

#include "Util/Vector3f.h"
#include "Util/Matrix4f.h"
#include "Util/File.h"
#include "Util/Log.h"

namespace Flux {
    namespace {
        const int LOG_SIZE = 1024;

        int loadShader(std::string path, int type) {
            String source = File::loadFile(path.c_str());

            const char* csource = source.c_str();

            // Create the shader
            GLuint shader;
            shader = glCreateShader(type);
            glShaderSource(shader, 1, &csource, NULL);

            return shader;
        }

        bool compileShader(std::string path, GLuint shader) {
            glCompileShader(shader);

            char log[LOG_SIZE];
            GLint status;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
            if (status == GL_FALSE) {
                glGetShaderInfoLog(shader, LOG_SIZE, nullptr, log);
                throw ShaderCompilationException(path, log);
            }
            return true;
        }

        bool linkProgram(const GLuint program) {
            glLinkProgram(program);

            GLint status = 0;
            glGetProgramiv(program, GL_LINK_STATUS, &status);
            
            return status == GL_TRUE;
        }

        bool validateProgram(const GLuint program) {
            glValidateProgram(program);

            GLint status = 0;
            glGetProgramiv(program, GL_VALIDATE_STATUS, &status);

            return status == GL_TRUE;
        }
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

    void Shader::uniform1iv(const char* name, int count, int* values) {
        glUniform1iv(location(name), count, (GLint*)values);
    }

    void Shader::uniform2i(const char* name, int v0, int v1) {
        glUniform2i(location(name), v0, v1);
    }

    void Shader::uniform1f(const char* name, float value) {
        glUniform1f(location(name), value);
    }

    void Shader::uniform1fv(const char* name, int count, float* values) {
        glUniform1fv(location(name), count, (GLfloat*)values);
    }

    void Shader::uniform2f(const char* name, float v0, float v1) {
        glUniform2f(location(name), v0, v1);
    }

    void Shader::uniform3f(const char* name, float v0, float v1, float v2) {
        glUniform3f(location(name), v0, v1, v2);
    }

    void Shader::uniform3f(const char* name, Vector3f v) {
        glUniform3f(location(name), v.x, v.y, v.z);
    }

    void Shader::uniform3fv(const char* name, int count, Vector3f* v) {
        glUniform3fv(location(name), count, (GLfloat*) v);
    }

    void Shader::uniformMatrix4f(const char* name, Matrix4f& m) {
        glUniformMatrix4fv(location(name), 1, false, m.toArray());
    }

    bool Shader::loadFromFile(std::string vertPath, std::string fragPath) {
        Log::info("Loading shader program: " + vertPath + " " + fragPath);
        int vertexShader = loadShader(vertPath, GL_VERTEX_SHADER);
        int fragmentShader = loadShader(fragPath, GL_FRAGMENT_SHADER);

        try {
            compileShader(vertPath, vertexShader);
            compileShader(fragPath, fragmentShader);
        }
        catch (const ShaderCompilationException& e) {
            Log::error(e.what());
            return false;
        }

        handle = glCreateProgram();

        glAttachShader(handle, vertexShader);
        glAttachShader(handle, fragmentShader);

        glBindAttribLocation(handle, 0, "position");
        glBindAttribLocation(handle, 1, "texCoords");
        glBindAttribLocation(handle, 2, "normal");
        glBindAttribLocation(handle, 3, "tangent");

        if (!linkProgram(handle)) {
            Log::error("Shader failed to link.");
            return false;
        }
        if (!validateProgram(handle)) {
            Log::error("Shader is not valid.");
            return false;
        }

        Log::info("Successfully compiled shader program: " + vertPath + " " + fragPath);

        return true;
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

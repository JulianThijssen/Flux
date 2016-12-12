/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** ShaderLoader.cpp
** Implements a class for loading vertex and fragment shaders and compiling
** them into a shader program.
**
** Author: Julian Thijssen
** -------------------------------------------------------------------------*/

#include "ShaderLoader.h"

#include "Exceptions/ShaderCompilationException.h"
#include "Exceptions/ShaderLinkException.h"

#include "Log.h"
#include "File.h"
#include "String.h"

#include <glad/glad.h>

#include <fstream>

namespace Flux {
    const int ShaderLoader::LOG_SIZE = 1024;

    Shader* ShaderLoader::loadShaderProgram(std::string vertPath, std::string fragPath) {
        Log::info("Loading shader program: " + vertPath + " " + fragPath);
        int vertexShader = loadShader(vertPath, GL_VERTEX_SHADER);
        int fragmentShader = loadShader(fragPath, GL_FRAGMENT_SHADER);

        int shaderProgram = glCreateProgram();

        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);

        glLinkProgram(shaderProgram);

        checkLinkStatus(vertPath, shaderProgram);

        glValidateProgram(shaderProgram);

        Log::info("Successfully compiled shader program: " + vertPath + " " + fragPath);

        Shader* shader = new Shader(shaderProgram);

        return shader;
    }

    int ShaderLoader::loadShader(std::string path, int type) {
        int handle = 0;

        

        String source = File::loadFile(path.c_str());

        const char* csource = source.c_str();

        // Create the shader
        handle = glCreateShader(type);
        glShaderSource(handle, 1, &csource, NULL);
        glCompileShader(handle);

        checkCompilationStatus(path, handle);

        return handle;
    }

    void ShaderLoader::checkCompilationStatus(std::string path,  GLuint shader) {
        char log[LOG_SIZE];
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            glGetShaderInfoLog(shader, LOG_SIZE, nullptr, log);
            throw ShaderCompilationException(path, log);
        }
    }

    void ShaderLoader::checkLinkStatus(std::string path, const GLuint program) {
        GLint status = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            throw ShaderLinkException();
        }
    }
}

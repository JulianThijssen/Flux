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

#include "Log.h"

#include <glad/glad.h>

#include <fstream>

const int ShaderLoader::LOG_SIZE = 1024;

Shader ShaderLoader::loadShaders(std::string vertPath, std::string fragPath) {
    int vertexShader = loadShader(vertPath, GL_VERTEX_SHADER);
    int fragmentShader = loadShader(fragPath, GL_FRAGMENT_SHADER);

    int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);
    glValidateProgram(shaderProgram);

    int handle = shaderProgram;

    Shader shader(handle);

    return shader;
}

int ShaderLoader::loadShader(std::string path, int type) {
    int handle = 0;

    Log::info("Loading shader file: " + path);
    // Read all lines and append together
    std::ifstream file(path);
    if (file.fail() || !file.is_open()) {
        Log::error("No such file: " + path);
    }

    std::string source;
    std::string line;
    while (std::getline(file, line)) {
        source.append(line + "\n");
    }

    const char* csource = source.c_str();

    // Create the shader
    handle = glCreateShader(type);
    glShaderSource(handle, 1, &csource, NULL);
    glCompileShader(handle);

    // Error checking
    char log[LOG_SIZE];
    GLint status;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderInfoLog(handle, LOG_SIZE, nullptr, log);
        Log::error(log);
    }
    else {
        Log::info("Successfully compiled shader: " + path);
    }

    return handle;
}

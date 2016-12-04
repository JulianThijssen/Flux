/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** ShaderLoader.h
** Declares a class for loading vertex and fragment shaders and compiling
** them into a shader program.
**
** Author: Julian Thijssen
** -------------------------------------------------------------------------*/

#pragma once
#ifndef SHADERLOADER_H
#define SHADERLOADER_H

#include "Shader.h"

#include <string>

namespace Flux {
    class ShaderLoader {
    public:
        static const int LOG_SIZE;

        static Shader* loadShaders(std::string vertPath, std::string fragPath);
    private:
        static int loadShader(std::string path, int type);
    };
}

#endif /* SHADERLOADER_H */

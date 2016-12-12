#pragma once
#ifndef SHADER_COMPILATION_EXCEPTION_H
#define SHADER_COMPILATION_EXCEPTION_H

#include <exception>
#include <stdexcept>
#include <string>

namespace Flux {
    class ShaderCompilationException : public std::runtime_error {
    public:
        ShaderCompilationException(std::string path, std::string error) : runtime_error("Shader failed to compile: " + path + " " + error) { }
    };
}

#endif /* SHADER_COMPILATION_EXCEPTION_H */

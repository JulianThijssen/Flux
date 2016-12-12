#pragma once
#ifndef SHADER_LINK_EXCEPTION_H
#define SHADER_LINK_EXCEPTION_H

#include <exception>
#include <stdexcept>
#include <string>

namespace Flux {
    class ShaderLinkException : public std::runtime_error {
    public:
        ShaderLinkException() : runtime_error("Shader failed to link") { }
    };
}

#endif /* SHADER_LINK_EXCEPTION_H */

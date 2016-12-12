#pragma once
#ifndef COMPONENT_NOT_FOUND_EXCEPTION_H
#define COMPONENT_NOT_FOUND_EXCEPTION_H

#include <exception>
#include <stdexcept>

namespace Flux {
    class ComponentNotFoundException : public std::runtime_error {
    public:
        ComponentNotFoundException() : runtime_error("Component not found") { }
    };
}

#endif /* COMPONENT_NOT_FOUND_EXCEPTION_H */

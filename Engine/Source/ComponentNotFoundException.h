#include <exception>
#include <stdexcept>

namespace Flux {
    class ComponentNotFoundException : public std::runtime_error {
    public:
        ComponentNotFoundException() : runtime_error("Component not found") { }
    };
}

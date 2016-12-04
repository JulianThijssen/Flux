#include "Component.h"

namespace Flux {
    class PointLight : public Component {
    public:
        PointLight() : energy(DEFAULT_ENERGY) { }

        const float DEFAULT_ENERGY = 1.0f;
        float energy;
    };
}

#pragma once
#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "Component.h"

namespace Flux {
    class PointLight : public Component {
    public:
        PointLight() : energy(DEFAULT_ENERGY) { }

        const float DEFAULT_ENERGY = 1.0f;
        float energy;
    };
}

#endif /* POINT_LIGHT_H */

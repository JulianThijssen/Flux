#pragma once
#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include "Component.h"
#include "Vector3f.h"

namespace Flux {
    class DirectionalLight : public Component {
    public:
        DirectionalLight()
        :   energy(DEFAULT_ENERGY)
        ,   direction(0, -1, 0)
        { }

        const float DEFAULT_ENERGY = 1.0f;

        float energy;
        Vector3f direction;
    };
}

#endif /* DIRECTIONAL_LIGHT_H */

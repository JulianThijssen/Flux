#pragma once
#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "Component.h"
#include "Vector3f.h"

namespace Flux {
    class PointLight : public Component {
    public:
        PointLight()
        :   energy(DEFAULT_ENERGY)
        ,   color(1, 1, 1)
        { }

        const float DEFAULT_ENERGY = 1.0f;

        Vector3f color;
        float energy;
    };
}

#endif /* POINT_LIGHT_H */

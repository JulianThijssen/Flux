#pragma once
#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "Component.h"
#include "Util/Vector3f.h"
#include "Framebuffer.h"

#include <memory>

namespace Flux {
    class PointLight : public Component {
    public:
        const Vector3f transforms[6] = {
            Vector3f(180, 90, 0),  // Positive X
            Vector3f(180, -90, 0), // Negative X
            Vector3f(90, 0, 0),    // Positive Y
            Vector3f(-90, 0, 0),   // Negative Y
            Vector3f(180, 0, 0),   // Positive Z
            Vector3f(180, 180, 0)  // Negative Z
        };

        PointLight()
            :
            energy(DEFAULT_ENERGY),
            color(1, 1, 1)
        { }

        const float DEFAULT_ENERGY = 1.0f;

        Vector3f color;
        float energy;

        Cubemap shadowMap;
        Framebuffer shadowBuffer;
    };
}

#endif /* POINT_LIGHT_H */

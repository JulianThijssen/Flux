#pragma once
#ifndef AREA_LIGHT_H
#define AREA_LIGHT_H

#include "Component.h"
#include "Util/Vector3f.h"
#include "Framebuffer.h"

#include <memory>

namespace Flux {
    class AreaLight : public Component {
    public:
        AreaLight()
            : energy(DEFAULT_ENERGY)
            , color(1, 1, 1)
        { }

        const float DEFAULT_ENERGY = 1.0f;

        Vector3f color;
        float energy;

        Cubemap* shadowMap;
        std::unique_ptr<Framebuffer> shadowBuffer;
    };
}

#endif /* AREA_LIGHT_H */

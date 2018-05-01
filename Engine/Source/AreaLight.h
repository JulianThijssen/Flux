#pragma once

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
        {
            vertices.push_back(Vector3f(-1, -1, 0));
            vertices.push_back(Vector3f(1, -1, 0));
            vertices.push_back(Vector3f(1, 1, 0));
            vertices.push_back(Vector3f(-1, 1, 0));
        }

        const float DEFAULT_ENERGY = 1.0f;

        Vector3f color;
        float energy;

        std::vector<Vector3f> vertices;

        Cubemap* shadowMap;
        std::unique_ptr<Framebuffer> shadowBuffer;
    };
}

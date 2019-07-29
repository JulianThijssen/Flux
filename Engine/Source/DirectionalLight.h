#pragma once

#include "Framebuffer.h"
#include "Component.h"

#include <GDT/Vector3f.h>
#include <GDT/Matrix4f.h>

#include <memory>

using GDT::Vector3f;
using GDT::Matrix4f;

namespace Flux {
    class DirectionalLight : public Component {
    public:
        DirectionalLight()
        :   energy(DEFAULT_ENERGY)
        ,   color(1, 1, 1)
        { }

        const float DEFAULT_ENERGY = 1.0f;

        float energy;
        Vector3f color;

        Texture2D shadowMap;
        Matrix4f shadowSpace;
        Framebuffer shadowBuffer;
    };
}

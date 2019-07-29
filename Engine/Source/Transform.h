#pragma once

#include "Component.h"

#include <GDT/Vector3f.h>

using GDT::Vector3f;

namespace Flux {
    class Transform : public Component {
    public:
        Transform()
        :    position(0, 0, 0)
        ,    rotation(0, 0, 0)
        ,    scale(1, 1, 1)
        {

        }

        Vector3f position;
        Vector3f rotation;
        Vector3f scale;
    };
}

#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Component.h"
#include "Vector3f.h"

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

#endif /* TRANSFORM_H */

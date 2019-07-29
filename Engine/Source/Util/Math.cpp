/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** Math.cpp
** Implements the supplementary math functions
**
** Author: Julian Thijssen
** -------------------------------------------------------------------------*/

#include "Math.h"

#include <GDT/Vector3f.h>
#include <GDT/Matrix4f.h>

using GDT::Vector3f;
using GDT::Matrix4f;

namespace Flux
{
    namespace Math
    {
        float toDegrees(const float radians)
        {
            return radians * (180 / PI);
        }

        float toRadians(const float degrees)
        {
            return degrees * (PI / 180);
        }

        Vector3f directionFromRotation(Vector3f rotation, Vector3f initialDirection, bool normalized)
        {
            Matrix4f yawMatrix;
            yawMatrix.rotate(rotation.y, 0, 1, 0);

            Matrix4f pitchMatrix;
            pitchMatrix.rotate(rotation.x, 1, 0, 0);

            Vector3f direction;
            direction = pitchMatrix.transform(initialDirection, 0);
            direction = yawMatrix.transform(direction, 0);

            if (normalized) {
                return direction.normalize();
            }
            return direction;
        }
    }
}

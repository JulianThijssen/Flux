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

#include "Matrix4f.h"
#include "Vector3f.h"

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

    float Math::toRadians(const float degrees) {
        return degrees * (PI / 180);
    }
}

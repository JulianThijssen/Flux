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

#include "Util/Math.h"

namespace Flux {
    const float Math::PI = 3.14159265359f;

    float Math::toDegrees(const float radians) {
        return radians * (180 / PI);
    }

    float Math::toRadians(const float degrees) {
        return degrees * (PI / 180);
    }
}

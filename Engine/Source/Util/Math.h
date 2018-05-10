/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** Math.h
** Declares the supplementary math functions
**
** Author: Julian Thijssen
** -------------------------------------------------------------------------*/

#pragma once

namespace Flux
{
    class Vector3f;

    namespace Math
    {
        const float PI = 3.14159265359f;

        float toDegrees(const float radians);
        float toRadians(const float degrees);
}

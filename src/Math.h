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
#ifndef MATH_H
#define MATH_H

namespace Flux {
    class Math {
    public:
        static const float PI;

        static float toDegrees(const float radians);
        static float toRadians(const float degrees);
    };
}

#endif /* MATH_H */

#pragma once

namespace GDT
{
    namespace Math
    {
        const float PI = 3.14159265359f;
        const float TWO_PI = 2 * 3.14159265359f;

        float min(float f1, float f2);
        float max(float f1, float f2);

        float toDegrees(const float radians);
        float toRadians(const float degrees);
    }
}

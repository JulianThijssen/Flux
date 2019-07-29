#pragma once

#include <iostream>

namespace GDT
{
    class Vector2f
    {
    public:
        float x, y;

        Vector2f();
        Vector2f(float x, float y);
        Vector2f(float xy);

        void set(float x, float y);
        void set(const Vector2f& v);
        Vector2f& normalize();

        float sqrMagnitude() const;
        float length() const;

        /* Operator overloads */
        bool operator==(const Vector2f& v) const;
        bool operator!=(const Vector2f& v) const;

        Vector2f& operator+=(const Vector2f& v);
        Vector2f& operator-=(const Vector2f& v);
        Vector2f& operator*=(const Vector2f& v);
        Vector2f& operator/=(const Vector2f& v);

        Vector2f& operator+=(const float f);
        Vector2f& operator-=(const float f);
        Vector2f& operator*=(const float f);
        Vector2f& operator/=(const float f);

        Vector2f operator+(const Vector2f& v) const;
        Vector2f operator-(const Vector2f& v) const;
        Vector2f operator*(const Vector2f& v) const;
        Vector2f operator/(const Vector2f& v) const;

        Vector2f operator+(const float f);
        Vector2f operator-(const float f);
        Vector2f operator*(const float f);
        Vector2f operator/(const float f);

        Vector2f operator-() const;
    };

    float dot(const Vector2f& v1, const Vector2f& v2);
    Vector2f normalize(const Vector2f& v);
    Vector2f pow(const Vector2f& v, float exponent);

    std::ostream& operator<<(std::ostream& os, const Vector2f& v);
}

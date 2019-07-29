#pragma once

#include <iostream>

namespace GDT
{
    class Vector3f;

    class Vector4f
    {
    public:
        float x, y, z, w;

        Vector4f();
        Vector4f(float x, float y, float z, float w);
        Vector4f(float xyz);
        Vector4f(float xyz, float w);
        Vector4f(Vector3f v, float w);

        void set(float x, float y, float z, float w);
        void set(const Vector4f& v);
        Vector4f& normalize();

        float sqrMagnitude() const;
        float length() const;

        /* Operator overloads */
        bool operator==(const Vector4f& v) const;
        bool operator!=(const Vector4f& v) const;
        float& operator[](size_t pos);
        const float& operator[](size_t pos) const;

        Vector4f& operator+=(const Vector4f& v);
        Vector4f& operator-=(const Vector4f& v);
        Vector4f& operator*=(const Vector4f& v);
        Vector4f& operator/=(const Vector4f& v);

        Vector4f& operator+=(const float f);
        Vector4f& operator-=(const float f);
        Vector4f& operator*=(const float f);
        Vector4f& operator/=(const float f);

        Vector4f operator+(const Vector4f& v) const;
        Vector4f operator-(const Vector4f& v) const;
        Vector4f operator*(const Vector4f& v) const;
        Vector4f operator/(const Vector4f& v) const;

        Vector4f operator+(const float f);
        Vector4f operator-(const float f);
        Vector4f operator*(const float f);
        Vector4f operator/(const float f);

        Vector4f operator-() const;
    };

    float dot(const Vector4f& v1, const Vector4f& v2);
    Vector4f cross(const Vector4f& v1, const Vector4f& v2);
    Vector4f normalize(const Vector4f& v);
    Vector4f pow(const Vector4f& v, float exponent);
    Vector4f mix(const Vector4f& v1, const Vector4f& v2, float a);
    float min(const Vector4f& v);
    float max(const Vector4f& v);

    std::ostream& operator<<(std::ostream& os, const Vector4f& v);
}

#pragma once

#include <string>

namespace GDT
{
    class Vector3f;
    class Vector4f;

    class Matrix4f {
    public:
        static const Matrix4f IDENTITY;
        static const Matrix4f BIAS;

        /* Core */
        Matrix4f();

        void setIdentity();
        void translate(const Vector3f& v);
        void rotate(float angle, float x, float y, float z);
        void rotate(const Vector3f& euler);
        void scale(float scale);
        void scale(const Vector3f& scale);
        Vector3f transform(const Vector3f& v, int w) const;

        const float* toArray() const;
        float* toArray();
        std::string str() const;

        /* Operator overloads */
        float operator[](int i) const;
        float& operator[](int i);
        bool operator==(const Matrix4f& m) const;
        bool operator!=(const Matrix4f& m) const;
        Matrix4f operator*(const Matrix4f& m) const;
        Vector4f operator*(const Vector4f& v) const;
        Vector3f operator*(const Vector3f& v) const;
    private:
        Matrix4f(float m0, float m1, float m2, float m3,
            float m4, float m5, float m6, float m7,
            float m8, float m9, float m10, float m11,
            float m12, float m13, float m14, float m15);

        float a[16];
    };


    /* Utility functions */
    Matrix4f transpose(const Matrix4f& m);
    float determinant(const Matrix4f& m);
    Matrix4f inverse(const Matrix4f& m);
}

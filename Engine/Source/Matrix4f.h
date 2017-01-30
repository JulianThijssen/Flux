/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** Matrix4f.h
** Declares a 4x4 matrix consisting of 16 float values and its helper functions
**
** Author: Julian Thijssen
** -------------------------------------------------------------------------*/

#pragma once
#ifndef MATRIX4F_H
#define MATRIX4F_H

#include <string>

namespace Flux {
    class Vector3f;

    class Matrix4f {
    public:
        /* Core */
        Matrix4f();
        void setIdentity();
        void translate(const Vector3f& v);
        void rotate(float angle, float x, float y, float z);
        void rotate(const Vector3f& euler);
        void scale(float scale);
        void scale(const Vector3f& scale);
        Vector3f transform(const Vector3f& v) const;
        
        float* toArray();
        std::string str() const;

        /* Operator overloads */
        float operator[](int i) const;
        float& operator[](int i);
        bool operator==(const Matrix4f& m) const;
        bool operator!=(const Matrix4f& m) const;
        Matrix4f operator*(const Matrix4f& m) const;
        Vector3f operator*(const Vector3f& m) const;
    private:
        float a[16];
    };


    /* Utility functions */
    Matrix4f transpose(const Matrix4f& m);
    float determinant(const Matrix4f& m);
    Matrix4f inverse(const Matrix4f& m);
}

#endif /* MATRIX4F_H */

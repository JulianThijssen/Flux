/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** Camera.h
** Declares a camera class containing functions perform perspective or
** orthographic transformations on a matrix
**
** Author: Julian Thijssen
** -------------------------------------------------------------------------*/

#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "Component.h"

namespace Flux {
    class Matrix4f;

    class Camera : public Component {
    public:
        Camera();
        Camera(float left, float right, float bottom, float top, float zNear, float zFar);
        Camera(float fovy, float aspect, float zNear, float zFar);
        void loadProjectionMatrix(Matrix4f& m);
        void setPerspective();
        void setOrthographic();

        float getFovy() const;
        float getAspectRatio() const;
        float getZNear() const;
        float getZFar() const;

        void setFovy(float fovy);
        void setAspectRatio(float aspect);
        void setZNear(float zNear);
        void setZFar(float zFar);

        void setBounds(float left, float right, float bottom, float top);

    private:
        const bool DEFAULT_PERSPECTIVE = true;
        const float DEFAULT_FOVY = 90;
        const float DEFAULT_ASPECT = 1;
        const float DEFAULT_ZNEAR = 0.1f;
        const float DEFAULT_ZFAR = 100;

        const float DEFAULT_LEFT = -1;
        const float DEFAULT_RIGHT = 1;
        const float DEFAULT_BOTTOM = -1;
        const float DEFAULT_TOP = 1;

        bool perspective = DEFAULT_PERSPECTIVE;
        float fovy = DEFAULT_FOVY;
        float aspect = DEFAULT_ASPECT;
        float zNear = DEFAULT_ZNEAR;
        float zFar = DEFAULT_ZFAR;

        float top = DEFAULT_TOP;
        float bottom = DEFAULT_BOTTOM;
        float left = DEFAULT_LEFT;
        float right = DEFAULT_RIGHT;
    };
}

#endif /* CAMERA_H */

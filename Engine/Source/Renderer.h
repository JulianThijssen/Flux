#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h"
#include "Model.h"
#include "Matrix4f.h"
#include "Scene.h"

namespace Flux {
    class Renderer {
    public:
        Renderer() : 
            clearColor(1, 0, 1),
            projMatrix(),
            viewMatrix(),
            modelMatrix(),
            shader(0) { }

        virtual void create() = 0;
        virtual void update(const Scene& scene) = 0;
        virtual void renderScene(const Scene& scene) = 0;
        virtual void renderMesh(Entity* entity) = 0;

        void setClearColor(float r, float g, float b, float a);
    protected:
        Vector3f clearColor;

        Matrix4f projMatrix;
        Matrix4f viewMatrix;
        Matrix4f modelMatrix;

        Shader* shader;
    };
}

#endif /* RENDERER_H */

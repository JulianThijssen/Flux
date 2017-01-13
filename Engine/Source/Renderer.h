#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h"
#include "Matrix4f.h"
#include "Scene.h"
#include "Skybox.h"

namespace Flux {
    const int DIFFUSE_UNIT = 0;
    const int NORMAL_UNIT = 1;
    const int ROUGHNESS_UNIT = 2;
    const int METALNESS_UNIT = 3;
    const int IRRADIANCE_UNIT = 4;
    const int PREFILTER_UNIT = 5;
    const int SCALEBIAS_UNIT = 6;

    class Renderer {
    public:
        Renderer() : 
            clearColor(1, 0, 1),
            projMatrix(),
            viewMatrix(),
            modelMatrix(),
            shader(0) { }

        virtual bool create() = 0;
        virtual void update(const Scene& scene) = 0;
        virtual void renderScene(const Scene& scene) = 0;
        virtual void uploadMaterial(const Material& material) = 0;
        virtual void renderMesh(const Scene& scene, Entity* entity) = 0;

        void setClearColor(float r, float g, float b, float a);
    protected:
        Vector3f clearColor;

        Matrix4f projMatrix;
        Matrix4f viewMatrix;
        Matrix4f modelMatrix;

        Shader* shader;

        Skybox* skybox;
    };
}

#endif /* RENDERER_H */

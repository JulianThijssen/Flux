#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h"
#include "Matrix4f.h"
#include "Scene.h"
#include "Skybox.h"

#include "Framebuffer.h"
#include "Texture.h"

#include <vector>

namespace Flux {
    const int TEX_UNIT_DIFFUSE = 0;
    const int TEX_UNIT_NORMAL = 1;
    const int TEX_UNIT_ROUGHNESS = 2;
    const int TEX_UNIT_METALNESS = 3;
    const int TEX_UNIT_IRRADIANCE = 4;
    const int TEX_UNIT_PREFILTER = 5;
    const int TEX_UNIT_SCALEBIAS = 6;

    class Renderer {
    public:
        Renderer() : 
            clearColor(1, 0, 1),
            projMatrix(),
            viewMatrix(),
            modelMatrix(),
            shader(0),
            currentFramebuffer(0)
        { }

        virtual bool create() = 0;
        virtual void onResize(unsigned int width, unsigned int height) = 0;
        virtual void update(const Scene& scene) = 0;
        virtual void renderScene(const Scene& scene) = 0;
        virtual void uploadMaterial(const Material& material) = 0;
        virtual void renderMesh(const Scene& scene, Entity* entity) = 0;

        void setClearColor(float r, float g, float b, float a);
        void setCamera(Entity& camera);
        void drawQuad();

        const Framebuffer& getCurrentFramebuffer();
        const Framebuffer& getOtherFramebuffer();
        void switchBuffers();
    protected:
        Vector3f clearColor;

        Matrix4f projMatrix;
        Matrix4f viewMatrix;
        Matrix4f modelMatrix;

        Shader* shader;

        Skybox* skybox;

        Framebuffer* hdrBuffer;
        std::vector<Framebuffer> backBuffers;
        unsigned int currentFramebuffer;
    };
}

#endif /* RENDERER_H */

#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h"
#include "Matrix4f.h"
#include "Scene.h"
#include "Skybox.h"

#include "RenderPhase.h"
#include "Framebuffer.h"
#include "Texture.h"
#include "Size.h"

#include <vector>
#include <unordered_map>
#include <queue>

namespace Flux {
    enum Capability {
        BLENDING = GL_BLEND,
        FACE_CULLING = GL_CULL_FACE,
        DEPTH_TEST = GL_DEPTH_TEST,
        STENCIL_TEST = GL_STENCIL_TEST,
        POLYGON_OFFSET = GL_POLYGON_OFFSET_FILL
    };

    class Renderer {
    public:
        Renderer() :
            clearColor(1, 0, 1),
            projMatrix(),
            viewMatrix(),
            modelMatrix(),
            currentFramebuffer(0),
            currentHdrFramebuffer(0),
            windowSize(800, 600)
        {
            glGenVertexArrays(1, &quadVao);
        }

        virtual bool create(const Scene& scene, const Size windowSize) = 0;
        virtual void onResize(const Size windowSize) = 0;
        virtual void update(const Scene& scene) = 0;
        virtual void renderScene(const Scene& scene, Shader& shader) = 0;
        virtual void renderMesh(const Scene& scene, Shader& shader, Entity* entity) = 0;

        void addRenderPhase(RenderPhase* phase);
        void enable(Capability capability);
        void disable(Capability capability);

        void setClearColor(float r, float g, float b, float a);
        void setCamera(Shader& shader, Entity& camera);
        void drawQuad() const;

        const Framebuffer& getCurrentFramebuffer();
        const Framebuffer& getCurrentHdrFramebuffer();
        const Framebuffer& getOtherFramebuffer();
        const Framebuffer& getOtherHdrFramebuffer();
        void switchBuffers();
        void switchHdrBuffers();

        static GLuint quadVao;
    protected:
        RenderState renderState;
        Vector3f clearColor;

        Matrix4f projMatrix;
        Matrix4f viewMatrix;
        Matrix4f modelMatrix;

        Size windowSize;

        Framebuffer* hdrBuffer;
        std::vector<Framebuffer> backBuffers;
        std::vector<Framebuffer> hdrBackBuffers;
        unsigned int currentFramebuffer;
        unsigned int currentHdrFramebuffer;
    private:
        std::queue<RenderPhase*> renderPhases;
    };
}

#endif /* RENDERER_H */

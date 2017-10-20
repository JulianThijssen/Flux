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
    enum ShaderName { IBL, DIRECT, SKYBOX, TEXTURE, FXAA, GAMMA, TONEMAP, SKYSPHERE, BLOOM, BLUR, SSAO, GBUFFER, DINDIRECT, DDIRECT, SHADOW, MULTIPLY, SSAOBLUR, ADD };

    enum RenderPhaseName { RP_INDIRECT, RP_DIRECT, RP_SKY, RP_BLOOM, RP_BLUR, RP_TONEMAP, RP_GAMMA, RP_ANTIALIAS, RP_SSAO };
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
            shader(0),
            currentFramebuffer(0),
            currentHdrFramebuffer(0),
            windowSize(800, 600)
        {
            glGenVertexArrays(1, &quadVao);
        }

        virtual bool create(const Scene& scene, const Size windowSize) = 0;
        virtual void onResize(const Size windowSize) = 0;
        virtual void update(const Scene& scene) = 0;
        virtual void renderScene(const Scene& scene) = 0;
        virtual void renderMesh(const Scene& scene, Entity* entity) = 0;

        void addShader(const ShaderName name, Shader* shader);
        void setShader(const ShaderName shader);
        bool validateShaders();

        void addRenderPhase(RenderPhase* phase);
        void enable(Capability capability);
        void disable(Capability capability);

        void setClearColor(float r, float g, float b, float a);
        void setCamera(Entity& camera);
        void drawQuad();

        const Framebuffer& getCurrentFramebuffer();
        const Framebuffer& getCurrentHdrFramebuffer();
        const Framebuffer& getOtherFramebuffer();
        const Framebuffer& getOtherHdrFramebuffer();
        void switchBuffers();
        void switchHdrBuffers();

        static GLuint quadVao;
    protected:
        Vector3f clearColor;

        Matrix4f projMatrix;
        Matrix4f viewMatrix;
        Matrix4f modelMatrix;

        Shader* shader;

        Size windowSize;


        Framebuffer* hdrBuffer;
        std::vector<Framebuffer> backBuffers;
        std::vector<Framebuffer> hdrBackBuffers;
        unsigned int currentFramebuffer;
        unsigned int currentHdrFramebuffer;
    private:
        std::unordered_map<ShaderName, Shader*> shaders;
        std::queue<RenderPhase*> renderPhases;
    };
}

#endif /* RENDERER_H */

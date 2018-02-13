#pragma once
#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H

#include "Renderer.h"
#include "Renderer/GBuffer.h"

#include "Cubemap.h"

#include "Texture.h"

#include <memory>

namespace Flux {
    class Size;

    class DeferredRenderer : public Renderer {
    public:
        DeferredRenderer() { }

        virtual bool create(const Scene& scene, const Size windowSize);
        virtual void onResize(const Size windowSize);
        virtual void update(const Scene& scene);
        virtual void renderScene(const Scene& scene, Shader& shader);
        virtual void renderMesh(const Scene& scene, Shader& shader, Entity* e);

    private:
        void createBackBuffers(const unsigned int width, const unsigned int height);
        void createShadowMaps(const Scene& scene);

        void renderDepth(const Scene& scene);
        void renderShadowMaps(const Scene& scene);
        void renderFramebuffer(const Framebuffer& framebuffer);

        Shader gBufferShader;
        Shader shadowShader;
        Shader textureShader;

        GBuffer gBuffer;
        Framebuffer hdrBuffer;
        Framebuffer ldrBuffer;

        std::vector<RenderPhase*> hdrPasses;
        std::vector<RenderPhase*> ldrPasses;
        RenderPhase* toneMapPass;
    };
}

#endif /* DEFERRED_RENDERER_H */

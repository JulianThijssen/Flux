#pragma once
#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H

#include "Renderer.h"

#include "Cubemap.h"
#include "Renderer/ImageBasedRendering.h"
#include "Renderer/SSAORendering.h"

#include "Texture.h"

#include <memory>

namespace Flux {
    class GBuffer {
    public:
        Texture* albedoTex;
        Texture* normalTex;
		Texture* positionTex;
		Texture* depthTex;
    };

    class DeferredRenderer : public Renderer {
    public:
        DeferredRenderer() { }

        virtual bool create(const Scene& scene, const Size windowSize);
        virtual void onResize(const Size windowSize);
        virtual void update(const Scene& scene);
        virtual void renderScene(const Scene& scene);
        virtual void renderMesh(const Scene& scene, Entity* e);

    private:
        void createGBuffer(const unsigned int width, const unsigned int height);
        void createBackBuffers(const unsigned int width, const unsigned int height);
        void createShadowMaps(const Scene& scene);
        void globalIllumination(const Scene& scene);
        void directLighting(const Scene& scene);
        void renderSky(const Scene& scene);
        void applyPostprocess();
        void renderShadowMaps(const Scene& scene);
        void renderFramebuffer(const Framebuffer& framebuffer);

        GBuffer gBufferInfo;
        std::unique_ptr<Framebuffer> gBuffer;
        std::unique_ptr<Framebuffer> shadowBuffer;
        IblSceneInfo iblSceneInfo;

        SsaoInfo ssaoInfo;
        std::vector<Framebuffer*> halfBuffers;
    };
}

#endif /* DEFERRED_RENDERER_H */

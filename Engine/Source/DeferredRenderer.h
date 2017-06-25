#pragma once
#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H

#include "Renderer.h"

#include "Cubemap.h"
#include "Renderer/ImageBasedRendering.h"
#include "Renderer/SSAORendering.h"

#include "Texture.h"

#include <unordered_map>
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

        virtual bool create(const Scene& scene);
        virtual void onResize(unsigned int width, unsigned int height);
        virtual void update(const Scene& scene);
        virtual void renderScene(const Scene& scene);
        virtual void renderMesh(const Scene& scene, Entity* e);

    private:
        void createGBuffer(const unsigned int width, const unsigned int height);
        void globalIllumination(const Scene& scene);
        void directLighting(const Scene& scene);
        void renderSky(const Scene& scene);
        void applyPostprocess();
        void renderFramebuffer(const Framebuffer& framebuffer);

        std::unordered_map<ShaderName, Shader*> shaders;

        GBuffer gBufferInfo;
        std::unique_ptr<Framebuffer> gBuffer;
        IblSceneInfo iblSceneInfo;
        SsaoInfo ssaoInfo;
    };
}

#endif /* DEFERRED_RENDERER_H */

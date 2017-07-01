#pragma once
#ifndef FORWARD_RENDERER_H
#define FORWARD_RENDERER_H

#include "Renderer.h"

#include "Cubemap.h"
#include "Renderer/ImageBasedRendering.h"
#include "Renderer/SSAORendering.h"

#include "Texture.h"

namespace Flux {
    class ForwardRenderer : public Renderer {
    public:
        ForwardRenderer() { }

        virtual bool create(const Scene& scene);
        virtual void onResize(unsigned int width, unsigned int height);
        virtual void update(const Scene& scene);
        virtual void renderScene(const Scene& scene);
        virtual void renderMesh(const Scene& scene, Entity* e);

        void globalIllumination(const Scene& scene);
        void directLighting(const Scene& scene);
        void renderSky(const Scene& scene);
        void applyPostprocess();
        void renderFramebuffer(const Framebuffer& framebuffer);
    private:
        IblSceneInfo iblSceneInfo;
        SsaoInfo ssaoInfo;
    };
}

#endif /* FORWARD_RENDERER_H */

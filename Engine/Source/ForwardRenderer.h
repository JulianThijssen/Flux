#pragma once
#ifndef FORWARD_RENDERER_H
#define FORWARD_RENDERER_H

#include "Renderer.h"

#include "Cubemap.h"
#include "Renderer/ImageBasedRendering.h"

#include "Texture.h"

namespace Flux {
    class ForwardRenderer : public Renderer {
    public:
        ForwardRenderer() { }

        virtual bool create();
        virtual void onResize(unsigned int width, unsigned int height);
        virtual void update(const Scene& scene);
        virtual void renderScene(const Scene& scene);
        virtual void renderMesh(const Scene& scene, Entity* e);

        void globalIllumination(const Scene& scene);
        void directLighting(const Scene& scene);
        void renderSky(const Scene& scene, bool useSkybox);
        void applyPostprocess();
        void renderFramebuffer(const Framebuffer& framebuffer);
    private:
        Shader* IBLShader;
        Shader* lightShader;
        Shader* skyboxShader;
        Shader* textureShader;
        Shader* fxaaShader;
        Shader* gammaShader;
        Shader* tonemapShader;
        Shader* skysphereShader;

        IblSceneInfo iblSceneInfo;

        Texture* hdrMap;
    };
}

#endif /* FORWARD_RENDERER_H */

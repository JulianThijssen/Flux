#pragma once
#ifndef FORWARD_RENDERER_H
#define FORWARD_RENDERER_H

#include "Renderer.h"

#include "Cubemap.h"
#include "Renderer/ImageBasedRendering.h"

namespace Flux {
    class ForwardRenderer : public Renderer {
    public:
        ForwardRenderer() { }

        virtual bool create();
        virtual void update(const Scene& scene);
        virtual void renderScene(const Scene& scene);
        virtual void uploadMaterial(const Material& material);
        virtual void renderMesh(const Scene& scene, Entity* e);

        void globalIllumination(const Scene& scene);
        void directLighting(const Scene& scene);
        void renderSkybox(const Scene& scene);
    private:
        Shader* IBLShader;
        Shader* lightShader;
        Shader* skyboxShader;
        Shader* textureShader;
        Shader* fxaaShader;

        IblSceneInfo iblSceneInfo;
    };
}

#endif /* FORWARD_RENDERER_H */

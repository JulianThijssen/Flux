#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h"
#include "Util/Matrix4f.h"
#include "Scene.h"
#include "Skybox.h"

#include "RenderPhase.h"
#include "Framebuffer.h"
#include "Texture.h"
#include "Util/Size.h"
#include "Renderer/RenderState.h"

#include <vector>
#include <unordered_map>
#include <queue>

namespace Flux {
    class Renderer {
    public:
        Renderer() :
            windowSize(800, 600)
        {
            
        }

        virtual bool create(const Scene& scene, const Size windowSize) = 0;
        virtual void onResize(const Size windowSize) = 0;
        virtual void update(const Scene& scene) = 0;
        virtual void renderScene(const Scene& scene, Shader& shader) = 0;
        virtual void renderMesh(const Scene& scene, Shader& shader, Entity* entity) = 0;

        const std::vector<RenderPhase*> getHdrPasses();
        const std::vector<RenderPhase*> getLdrPasses();
        RenderPhase* getToneMapPass();

        void addHdrPass(RenderPhase* hdrPass);
        void addLdrPass(RenderPhase* ldrPass);
        void setToneMapPass(RenderPhase* tonemapPass);

    protected:
        RenderState renderState;

        Size windowSize;

        std::vector<Framebuffer> backBuffers;
        std::vector<Framebuffer> hdrBackBuffers;
    private:
        std::vector<RenderPhase*> hdrPasses;
        std::vector<RenderPhase*> ldrPasses;
        RenderPhase* toneMapPass;
    };
}

#endif /* RENDERER_H */

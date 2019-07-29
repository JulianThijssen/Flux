#pragma once

#include "Scene.h"

#include "Renderer/RenderState.h"
#include "RenderPhase.h"
#include "Renderer/TonemapPass.h"

#include "Framebuffer.h"
#include "Util/Size.h"

#include <GDT/Shader.h>

#include <vector>
#include <memory>

using GDT::ShaderProgram;

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
        virtual void renderScene(const Scene& scene, ShaderProgram& shader) = 0;
        virtual void renderMesh(const Scene& scene, ShaderProgram& shader, Entity* entity) = 0;

        const std::vector<std::unique_ptr<RenderPhase>>& getHdrPasses();
        const std::vector<std::unique_ptr<RenderPhase>>& getLdrPasses();
        TonemapPass& getToneMapPass();

        void addHdrPass(std::unique_ptr<RenderPhase> hdrPass);
        void addLdrPass(std::unique_ptr<RenderPhase> ldrPass);
        void setToneMapPass(std::unique_ptr<TonemapPass> tonemapPass);

    protected:
        RenderState renderState;

        Size windowSize;

        std::vector<Framebuffer> backBuffers;
        std::vector<Framebuffer> hdrBackBuffers;
    private:
        std::vector<std::unique_ptr<RenderPhase>> hdrPasses;
        std::vector<std::unique_ptr<RenderPhase>> ldrPasses;
        std::unique_ptr<TonemapPass> toneMapPass;
    };
}

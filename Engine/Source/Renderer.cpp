#include "Renderer.h"

#include <glad/glad.h>

namespace Flux {
    const std::vector<std::unique_ptr<RenderPhase>>& Renderer::getHdrPasses()
    {
        return hdrPasses;
    }

    const std::vector<std::unique_ptr<RenderPhase>>& Renderer::getLdrPasses()
    {
        return ldrPasses;
    }

    TonemapPass& Renderer::getToneMapPass()
    {
        return *toneMapPass;
    }

    void Renderer::addHdrPass(std::unique_ptr<RenderPhase> hdrPass)
    {
        hdrPasses.push_back(std::move(hdrPass));
    }

    void Renderer::addLdrPass(std::unique_ptr<RenderPhase> ldrPass)
    {
        ldrPasses.push_back(std::move(ldrPass));
    }

    void Renderer::setToneMapPass(std::unique_ptr<TonemapPass> toneMapPass)
    {
        this->toneMapPass.reset(toneMapPass.release());
    }
}

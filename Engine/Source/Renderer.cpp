#include "Renderer.h"

#include <glad/glad.h>

namespace Flux {
    const std::vector<RenderPhase*> Renderer::getHdrPasses()
    {
        return hdrPasses;
    }

    const std::vector<RenderPhase*> Renderer::getLdrPasses()
    {
        return ldrPasses;
    }

    RenderPhase* Renderer::getToneMapPass()
    {
        return toneMapPass;
    }

    void Renderer::addHdrPass(RenderPhase* hdrPass)
    {
        hdrPasses.push_back(hdrPass);
    }

    void Renderer::addLdrPass(RenderPhase* ldrPass)
    {
        ldrPasses.push_back(ldrPass);
    }

    void Renderer::setToneMapPass(RenderPhase* toneMapPass)
    {
        this->toneMapPass = toneMapPass;
    }
}

#include "Renderer/GammaCorrectionPass.h"

#include "Renderer/RenderState.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace Flux {
    GammaCorrectionPass::GammaCorrectionPass() : RenderPhase("Gamma Correction")
    {
        shader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/GammaCorrection.frag");
    }

    void GammaCorrectionPass::SetSource(const Texture* source)
    {
        this->source = source;
    }

    void GammaCorrectionPass::SetTarget(const Framebuffer* target)
    {
        this->target = target;
    }

    void GammaCorrectionPass::render(RenderState& renderState, const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        shader.bind();

        source->bind(TextureUnit::TEXTURE);
        shader.uniform1i("tex", TextureUnit::TEXTURE);
        target->bind();
        renderState.drawQuad();

        nvtxRangePop();
    }
}

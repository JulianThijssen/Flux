#include "Renderer/GammaCorrectionPass.h"

#include "Renderer/RenderState.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace Flux {
    GammaCorrectionPass::GammaCorrectionPass() : RenderPhase("Gamma Correction")
    {
        shader = std::unique_ptr<Shader>(Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/GammaCorrection.frag"));
    }

    void GammaCorrectionPass::SetSource(const Texture* source)
    {
        this->source = source;
    }

    void GammaCorrectionPass::SetTarget(const Framebuffer* target)
    {
        this->target = target;
    }

    void GammaCorrectionPass::render(const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        shader->bind();

        source->bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        target->bind();
        RenderState::drawQuad();

        nvtxRangePop();
    }
}

#include "Renderer/FxaaPass.h"

#include "Renderer/RenderState.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace Flux {
    FxaaPass::FxaaPass() : RenderPhase("FXAA"), windowSize(1, 1)
    {
        shader = std::unique_ptr<Shader>(Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/FXAAQuality.frag"));
    }

    void FxaaPass::SetSource(const Texture* source)
    {
        this->source = source;
    }

    void FxaaPass::SetTarget(const Framebuffer* target)
    {
        this->target = target;
    }

    void FxaaPass::Resize(const Size& windowSize)
    {
        this->windowSize = windowSize;
    }

    void FxaaPass::render(RenderState& renderState, const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        shader->bind();

        source->bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        glGenerateMipmap(GL_TEXTURE_2D);
        shader->uniform2f("rcpScreenSize", 1.0f / windowSize.width, 1.0f / windowSize.height);
        target->bind();
        renderState.drawQuad();

        nvtxRangePop();
    }
}

#include "Renderer/FxaaPass.h"

#include "Renderer/RenderState.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace Flux {
    FxaaPass::FxaaPass() : RenderPhase("FXAA"), windowSize(1, 1)
    {
        shader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/FXAAQuality.frag");

        requiredSet.addCapability(STENCIL_TEST, false);
        requiredSet.addCapability(DEPTH_TEST, false);
    }

    void FxaaPass::Resize(const Size& windowSize)
    {
        this->windowSize = windowSize;
    }

    void FxaaPass::render(RenderState& renderState, const Scene&)
    {
        renderState.require(requiredSet);

        nvtxRangePushA(getPassName().c_str());

        shader.bind();

        source->bind(TextureUnit::TEXTURE);
        shader.uniform1i("tex", TextureUnit::TEXTURE);
        glGenerateMipmap(GL_TEXTURE_2D);
        shader.uniform2f("rcpScreenSize", 1.0f / windowSize.width, 1.0f / windowSize.height);

        renderState.drawQuad();

        nvtxRangePop();
    }
}

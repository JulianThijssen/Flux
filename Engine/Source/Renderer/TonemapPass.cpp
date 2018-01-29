#include "Renderer/TonemapPass.h"

#include "Renderer.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace Flux {
    TonemapPass::TonemapPass() : RenderPhase("Tonemap")
    {
        shader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/Tonemap.frag");
    }

    void TonemapPass::Resize(const Size& windowSize)
    {

    }

    void TonemapPass::render(RenderState& renderState, const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        shader.bind();

        source->bind(TextureUnit::TEXTURE);
        shader.uniform1i("tex", TextureUnit::TEXTURE);

        renderState.drawQuad();

        nvtxRangePop();
    }
}

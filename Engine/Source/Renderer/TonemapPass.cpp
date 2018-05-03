#include "Renderer/TonemapPass.h"

#include "Renderer.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace Flux {
    TonemapPass::TonemapPass() : RenderPhase("Tonemap")
    {
        shader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/Tonemap.frag");

        requiredSet.addCapability(STENCIL_TEST, false);
        requiredSet.addCapability(DEPTH_TEST, false);
    }

    void TonemapPass::setTonemapper(Tonemapper tonemapper)
    {
        this->tonemapper = tonemapper;
    }

    void TonemapPass::setExposure(float exposure)
    {
        this->exposure = exposure;
    }

    void TonemapPass::Resize(const Size& windowSize)
    {

    }

    void TonemapPass::render(RenderState& renderState, const Scene& scene)
    {
        renderState.require(requiredSet);

        nvtxRangePushA(getPassName().c_str());

        shader.bind();

        source->bind(TextureUnit::TEXTURE);
        shader.uniform1i("source", TextureUnit::TEXTURE);

        shader.uniform1i("tonemapper", tonemapper);
        shader.uniform1f("exposure", exposure);

        renderState.drawQuad();

        nvtxRangePop();
    }
}

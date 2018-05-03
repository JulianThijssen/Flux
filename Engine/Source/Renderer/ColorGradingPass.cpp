#include "Renderer/ColorGradingPass.h"

#include "Renderer/RenderState.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Framebuffer.h"

#include "Util/Path.h"

namespace Flux {
    ColorGradingPass::ColorGradingPass() : RenderPhase("Color Grading")
    {
        shader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/ColorGrading.frag");
        lut.loadFromFile(Path("res/reinhart_grading.png"), COLOR);
        lut.setWrapping(CLAMP, CLAMP, CLAMP);
        lut.setSampling(LINEAR, LINEAR);

        requiredSet.addCapability(STENCIL_TEST, true);
        requiredSet.addCapability(DEPTH_TEST, false);
    }

    void ColorGradingPass::Resize(const Size& windowSize)
    {

    }

    void ColorGradingPass::render(RenderState& renderState, const Scene& scene)
    {
        renderState.require(requiredSet);

        nvtxRangePushA(getPassName().c_str());

        shader.bind();

        source->bind(TextureUnit::TEXTURE0);
        shader.uniform1i("tex", TextureUnit::TEXTURE0);
        lut.bind(TextureUnit::TEXTURE1);
        shader.uniform1i("lut", TextureUnit::TEXTURE1);

        renderState.drawQuad();

        nvtxRangePop();
    }
}

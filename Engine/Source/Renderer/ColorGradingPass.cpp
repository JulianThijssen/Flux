#include "Renderer/ColorGradingPass.h"

#include "Renderer/RenderState.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Texture3D.h"
#include "Framebuffer.h"

#include "TextureLoader.h"
#include "Util/Path.h"

namespace Flux {
    ColorGradingPass::ColorGradingPass() : RenderPhase("Color Grading")
    {
        shader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/ColorGrading.frag");
        lut = TextureLoader::loadTexture3D(Path("res/reinhart_grading.png"));
    }

    void ColorGradingPass::Resize(const Size& windowSize)
    {

    }

    void ColorGradingPass::render(RenderState& renderState, const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        shader.bind();

        source->bind(TextureUnit::TEXTURE0);
        shader.uniform1i("tex", TextureUnit::TEXTURE0);
        lut->bind(TextureUnit::TEXTURE1);
        shader.uniform1i("lut", TextureUnit::TEXTURE1);

        renderState.drawQuad();

        nvtxRangePop();
    }
}

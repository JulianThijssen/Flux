#include "Renderer/BloomPass.h"

#include "Renderer.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace Flux {
    BloomPass::BloomPass() : RenderPhase("Bloom")
    {
        shader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/Bloom.frag");
    }

    void BloomPass::Resize(const Size& windowSize)
    {
        for (int i = 0; i < 2; i++) {
            Framebuffer framebuffer;
            framebuffer.create();
            framebuffer.bind();
            // Textures are linearly sampled for first step of gaussian bloom blur
            framebuffer.addColorTexture(0, TextureLoader::create(windowSize.width, windowSize.height, GL_RGBA16F, GL_RGBA, GL_FLOAT, CLAMP, SamplingConfig(LINEAR, LINEAR, LINEAR)));
            framebuffer.validate();
            framebuffer.release();
            buffers.push_back(framebuffer);
        }
    }

    void BloomPass::render(RenderState& renderState, const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        shader.bind();

        source->bind(TextureUnit::TEXTURE);
        glGenerateMipmap(GL_TEXTURE_2D);
        shader.uniform1i("tex", TextureUnit::TEXTURE);
        shader.uniform1f("threshold", 0);

        buffers[0].bind();

        renderState.drawQuad();

        renderState.hdrBuffer.bind();

        std::vector<Texture> sources{ renderState.hdrBuffer.getColorTexture(0), buffers[0].getColorTexture(0) };
        std::vector<float> weights{ 1, 1 };
        addPass.SetTextures(sources);
        addPass.SetWeights(weights);
        addPass.render(renderState, scene);

        nvtxRangePop();
    }
}

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
        buffer.create();
        buffer.bind();
        // Textures are linearly sampled for first step of gaussian bloom blur
        buffer.addColorTexture(0, TextureLoader::create(windowSize.width, windowSize.height, GL_RGBA16F, GL_RGBA, GL_FLOAT, CLAMP, SamplingConfig(LINEAR, LINEAR, LINEAR)));
        buffer.validate();
        buffer.release();

        blurPass.Resize(windowSize);
    }

    void BloomPass::render(RenderState& renderState, const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        const Framebuffer* sourceFramebuffer = RenderState::currentFramebuffer;

        shader.bind();

        source->bind(TextureUnit::TEXTURE);

        shader.uniform1i("tex", TextureUnit::TEXTURE);
        shader.uniform1f("threshold", 0);

        buffer.bind();

        renderState.drawQuad();

        // Blur the buffer
        blurPass.SetSource(&buffer.getTexture());
        blurPass.render(renderState, scene);

        // Add the bloom to the HDR buffer
        sourceFramebuffer->bind();

        std::vector<Texture2D> sources{ *source, buffer.getTexture() };
        std::vector<float> weights{ 1, 0.5f };
        addPass.SetTextures(sources);
        addPass.SetWeights(weights);
        addPass.render(renderState, scene);

        nvtxRangePop();
    }
}

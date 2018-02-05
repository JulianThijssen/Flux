#include "Renderer/GaussianBlurPass.h"

#include "Renderer.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace Flux {
    GaussianBlurPass::GaussianBlurPass() : RenderPhase("GaussianBlur"), windowSize(1, 1)
    {
        shader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/BlurFast.frag");
    }

    void GaussianBlurPass::Resize(const Size& windowSize) {
        this->windowSize = windowSize;

        int blurWidth = windowSize.width;
        int blurHeight = windowSize.height;
        blurBuffers.resize(8);
        for (unsigned int i = 0; i < blurBuffers.size(); i++) {
            if (i % 2 == 0) {
                blurWidth = blurWidth >> 1; blurHeight = blurHeight >> 1;
            }

            blurBuffers[i].create();
            blurBuffers[i].bind();
            blurBuffers[i].addColorTexture(0, TextureLoader::create(blurWidth, blurHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT, CLAMP, SamplingConfig(LINEAR, LINEAR, NONE)));
            blurBuffers[i].validate();
            blurBuffers[i].release();
        }
    }

    void GaussianBlurPass::render(RenderState& renderState, const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());
        
        const Framebuffer* sourceFramebuffer = RenderState::currentFramebuffer;

        shader.bind();

        source->bind(TextureUnit::TEXTURE);
        glGenerateMipmap(GL_TEXTURE_2D);

        shader.uniform1i("tex", TextureUnit::TEXTURE);

        shader.uniform2f("direction", 1, 0);
        for (unsigned int i = 0; i < blurBuffers.size(); i += 2) {
            const Texture& texture = blurBuffers[i].getColorTexture(0);
            int width = texture.getWidth();
            int height = texture.getHeight();
            glViewport(0, 0, width, height);
            shader.uniform2i("windowSize", width, height);
            blurBuffers[i].bind();
            shader.uniform1i("mipmap", i + 1);

            renderState.drawQuad();
        }
        shader.uniform2f("direction", 0, 1);
        for (unsigned int i = 1; i < blurBuffers.size(); i += 2) {
            const Texture& texture = blurBuffers[i-1].getColorTexture(0);
            texture.bind(TextureUnit::TEXTURE);
            int width = texture.getWidth();
            int height = texture.getHeight();
            glViewport(0, 0, width, height);
            shader.uniform2i("windowSize", width, height);
            blurBuffers[i].bind();
            shader.uniform1i("mipmap", 0);

            renderState.drawQuad();
        }
        nvtxRangePop();

        sourceFramebuffer->bind();

        glViewport(0, 0, windowSize.width, windowSize.height);

        std::vector<Texture> v = {
            blurBuffers[1].getColorTexture(0),
            blurBuffers[3].getColorTexture(0),
            blurBuffers[5].getColorTexture(0),
            blurBuffers[7].getColorTexture(0),
        };
        std::vector<float> weights{ 0.25f, 0.25f, 0.25f, 0.25f };
        averagePass.SetTextures(v);
        averagePass.SetWeights(weights);
        averagePass.render(renderState, scene);
    }
}

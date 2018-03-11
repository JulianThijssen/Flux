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
        blurBuffers.resize(6);
        for (unsigned int i = 0; i < blurBuffers.size(); i++) {
            if (i % 2 == 0) {
                blurWidth = blurWidth >> 1; blurHeight = blurHeight >> 1;
            }

            Texture2D blurTexture;
            blurTexture.create();
            blurTexture.bind(TextureUnit::TEXTURE0);
            blurTexture.setData(blurWidth, blurHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT, nullptr);
            blurTexture.setWrapping(CLAMP, CLAMP);
            blurTexture.setSampling(LINEAR, LINEAR);
            blurTexture.release();

            blurBuffers[i].create();
            blurBuffers[i].bind();
            blurBuffers[i].addColorTexture(0, blurTexture);
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
            const Texture2D& texture = blurBuffers[i].getTexture();
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
            const Texture2D& texture = blurBuffers[i-1].getTexture();
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

        std::vector<Texture2D> v = {
            blurBuffers[1].getTexture(),
            blurBuffers[3].getTexture(),
            blurBuffers[5].getTexture(),
        };
        std::vector<float> weights{ 0.33f, 0.33f, 0.34f };
        averagePass.SetTextures(v);
        averagePass.SetWeights(weights);
        averagePass.render(renderState, scene);
    }
}

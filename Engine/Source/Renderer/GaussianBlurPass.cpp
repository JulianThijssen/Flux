#include "Renderer/GaussianBlurPass.h"

#include "Renderer/AveragePass.h"

#include "Renderer.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace Flux {
    GaussianBlurPass::GaussianBlurPass() : RenderPhase("GaussianBlur"), windowSize(1, 1)
    {
        shader = std::unique_ptr<Shader>(Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/BlurFast.frag"));

        averagePass = new AveragePass();
    }

    void GaussianBlurPass::Resize(const Size& windowSize) {
        this->windowSize = windowSize;

        int blurWidth = windowSize.width;
        int blurHeight = windowSize.height;
        blurBuffers.resize(4);
        blurBuffers2.resize(4);
        for (unsigned int i = 0; i < blurBuffers.size(); i++) {
            blurWidth = blurWidth >> 1; blurHeight = blurHeight >> 1;
            blurBuffers[i] = new Framebuffer(blurWidth, blurHeight);
            blurBuffers[i]->bind();
            blurBuffers[i]->addColorTexture(0, TextureLoader::createEmpty(blurWidth, blurHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT, Sampling::LINEAR, false));
            blurBuffers[i]->validate();
            blurBuffers[i]->release();
            blurBuffers2[i] = new Framebuffer(blurWidth, blurHeight);
            blurBuffers2[i]->bind();
            blurBuffers2[i]->addColorTexture(0, TextureLoader::createEmpty(blurWidth, blurHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT, Sampling::LINEAR, false));
            blurBuffers2[i]->validate();
            blurBuffers2[i]->release();
        }
    }

    void GaussianBlurPass::SetSource(const Texture* source)
    {
        this->source = source;
    }

    void GaussianBlurPass::SetTarget(const Framebuffer* target)
    {
        this->target = target;
    }

    void GaussianBlurPass::render(const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());
        
        shader->bind();

        source->bind(TextureUnit::TEXTURE);
        glGenerateMipmap(GL_TEXTURE_2D);

        shader->uniform1i("tex", TextureUnit::TEXTURE);

        shader->uniform2f("direction", 1, 0);
        for (unsigned int i = 0; i < blurBuffers.size(); i++) {
            const Texture& texture = blurBuffers[i]->getColorTexture(0);
            int width = texture.getWidth();
            int height = texture.getHeight();
            glViewport(0, 0, width, height);
            shader->uniform2i("windowSize", width, height);
            blurBuffers[i]->bind();
            shader->uniform1i("mipmap", i + 1);

            glBindVertexArray(Renderer::quadVao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        shader->uniform2f("direction", 0, 1);
        for (unsigned int i = 0; i < blurBuffers2.size(); i++) {
            const Texture& texture = blurBuffers[i]->getColorTexture(0);
            texture.bind(TextureUnit::TEXTURE);
            int width = texture.getWidth();
            int height = texture.getHeight();
            glViewport(0, 0, width, height);
            shader->uniform2i("windowSize", width, height);
            blurBuffers2[i]->bind();
            shader->uniform1i("mipmap", 0);

            glBindVertexArray(Renderer::quadVao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        nvtxRangePop();


        glViewport(0, 0, windowSize.width, windowSize.height);

        target->bind();

        std::vector<Texture> v = {
            blurBuffers2[0]->getColorTexture(0),
            blurBuffers2[1]->getColorTexture(0),
            blurBuffers2[2]->getColorTexture(0),
            blurBuffers2[3]->getColorTexture(0),
        };
        averagePass->SetTextures(v);
        averagePass->render(scene);
    }
}

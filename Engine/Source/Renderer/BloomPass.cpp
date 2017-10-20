#include "Renderer/BloomPass.h"

#include "Renderer.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace Flux {
    BloomPass::BloomPass() : RenderPhase("Bloom")
    {
        shader = std::unique_ptr<Shader>(Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Bloom.frag"));
    }

    void BloomPass::SetSource(const Texture* source)
    {
        this->source = source;
    }

    void BloomPass::SetTarget(const Framebuffer* target)
    {
        this->target = target;
    }

    void BloomPass::render(const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        shader->bind();

        source->bind(TextureUnit::TEXTURE);
        glGenerateMipmap(GL_TEXTURE_2D);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        shader->uniform1f("threshold", 0);

        target->bind();
        glBindVertexArray(Renderer::quadVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        nvtxRangePop();
    }
}

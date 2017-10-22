#include "Renderer/TonemapPass.h"

#include "Renderer.h"

#include "TextureUnit.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace Flux {
    TonemapPass::TonemapPass() : RenderPhase("Tonemap")
    {
        shader = std::unique_ptr<Shader>(Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Tonemap.frag"));
    }

    void TonemapPass::SetSource(const Texture* source)
    {
        this->source = source;
    }

    void TonemapPass::SetBloom(const Texture* bloom)
    {
        this->bloom = bloom;
    }

    void TonemapPass::SetTarget(const Framebuffer* target)
    {
        this->target = target;
    }

    void TonemapPass::render(const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        shader->bind();

        source->bind(TextureUnit::TEXTURE);
        shader->uniform1i("tex", TextureUnit::TEXTURE);
        bloom->bind(TextureUnit::BLOOM);
        shader->uniform1i("bloomTex", TextureUnit::BLOOM);

        target->bind();

        glBindVertexArray(Renderer::quadVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        nvtxRangePop();
    }
}

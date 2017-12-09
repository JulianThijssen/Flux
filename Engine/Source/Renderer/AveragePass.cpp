#include "Renderer/AveragePass.h"

#include "Texture.h"
#include "TextureUnit.h"

#include "Renderer.h"

#include <glad/glad.h>

namespace Flux {
    AveragePass::AveragePass() : RenderPhase("Average")
    {
        shader = std::unique_ptr<Shader>(Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Average.frag"));
    }

    void AveragePass::SetTextures(std::vector<Texture> textures)
    {
        this->textures = textures;

        units.clear();
        for (int i = 0; i < textures.size(); i++) {
            units.push_back(TextureUnit::TEXTURE0 + i);
        }
    }

    void AveragePass::render(RenderState& renderState, const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        shader->bind();

        const unsigned int sourceCount = textures.size();

        for (int i = 0; i < sourceCount; i++) {
            textures[i].bind(TextureUnit::TEXTURE0 + i);
        }
        shader->uniform1i("sourceCount", sourceCount);
        shader->uniform1iv("sources", sourceCount, units.data());

        renderState.drawQuad();

        nvtxRangePop();
    }
}

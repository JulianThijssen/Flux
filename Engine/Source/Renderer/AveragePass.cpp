#include "AveragePass.h"

#include <Engine/Source/Texture.h>
#include <Engine/Source/TextureUnit.h>

#include <Engine/Source/Renderer.h>

#include <glad/glad.h>


#include "nvToolsExt.h"

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

    void AveragePass::render(const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        shader->bind();

        const unsigned int sourceCount = textures.size();

        for (int i = 0; i < sourceCount; i++) {
            textures[i].bind(TextureUnit::TEXTURE0 + i);
        }
        shader->uniform1i("sourceCount", sourceCount);
        shader->uniform1iv("sources", sourceCount, units.data());

        glBindVertexArray(Renderer::quadVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        nvtxRangePop();
    }
}

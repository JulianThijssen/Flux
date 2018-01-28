#include "Renderer/AddPass.h"

#include "Texture.h"
#include "TextureUnit.h"

#include "Renderer.h"

#include <glad/glad.h>

namespace Flux {
    AddPass::AddPass() : RenderPhase("Add")
    {
        shader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/Add.frag");
    }

    void AddPass::SetTextures(std::vector<Texture> textures)
    {
        this->textures = textures;

        units.clear();
        for (int i = 0; i < textures.size(); i++) {
            units.push_back(TextureUnit::TEXTURE0 + i);
        }
    }

    void AddPass::SetWeights(std::vector<float> weights)
    {
        this->weights = weights;
    }

    void AddPass::Resize(const Size& windowSize)
    {

    }

    void AddPass::render(RenderState& renderState, const Scene& scene)
    {
        nvtxRangePushA(getPassName().c_str());

        shader.bind();

        const unsigned int sourceCount = textures.size();

        for (int i = 0; i < sourceCount; i++) {
            textures[i].bind(TextureUnit::TEXTURE0 + i);
        }
        shader.uniform1i("sourceCount", sourceCount);
        shader.uniform1iv("sources", sourceCount, units.data());

        shader.uniform1fv("weights", sourceCount, weights.data());

        renderState.drawQuad();

        nvtxRangePop();
    }
}

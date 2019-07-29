#pragma once

#include "RenderPhase.h"

#include <memory>

namespace Flux
{
    class Texture2D;

    class AddPass : public RenderPhase
    {
    public:
        AddPass();

        void SetTextures(std::vector<Texture2D>);
        void SetWeights(std::vector<float>);

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        static const unsigned int MAX_SOURCES = 8;

        ShaderProgram shader;

        std::vector<Texture2D> textures;
        std::vector<float> weights;

        std::vector<int> units;
    };
}

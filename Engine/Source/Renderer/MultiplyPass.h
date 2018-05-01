#pragma once

#include "RenderPhase.h"

#include "Shader.h"

#include <memory>

namespace Flux
{
    class Texture2D;

    class MultiplyPass : public RenderPhase
    {
    public:
        MultiplyPass();

        void SetTextures(std::vector<Texture2D>);

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        static const unsigned int MAX_SOURCES = 8;

        Shader shader;

        std::vector<Texture2D> textures;

        std::vector<int> units;
    };
}

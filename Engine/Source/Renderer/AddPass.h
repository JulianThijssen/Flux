#pragma once
#ifndef ADD_PASS_H
#define ADD_PASS_H

#include "RenderPhase.h"

#include "Shader.h"

#include <memory>

namespace Flux
{
    class Texture;

    class AddPass : public RenderPhase
    {
    public:
        AddPass();

        void SetTextures(std::vector<Texture>);
        void SetWeights(std::vector<float>);

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        static const unsigned int MAX_SOURCES = 8;

        Shader shader;

        std::vector<Texture> textures;
        std::vector<float> weights;

        std::vector<int> units;
    };
}

#endif /* ADD_PASS_H */

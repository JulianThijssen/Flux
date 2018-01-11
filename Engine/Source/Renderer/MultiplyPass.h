#pragma once
#ifndef MULTIPLY_PASS_H
#define MULTIPLY_PASS_H

#include "RenderPhase.h"

#include "Shader.h"

#include <memory>

namespace Flux
{
    class Texture;

    class MultiplyPass : public RenderPhase
    {
    public:
        MultiplyPass();

        void SetTextures(std::vector<Texture>);
        void render(RenderState& renderState, const Scene& scene) override;

    private:
        static const unsigned int MAX_SOURCES = 8;

        Shader shader;

        std::vector<Texture> textures;

        std::vector<int> units;
    };
}

#endif /* MULTIPLY_PASS_H */

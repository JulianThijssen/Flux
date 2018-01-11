#pragma once
#ifndef AVERAGE_PASS_H
#define AVERAGE_PASS_H

#include "RenderPhase.h"

#include "Shader.h"

#include <memory>

namespace Flux
{
    class Texture;

    class AveragePass : public RenderPhase
    {
    public:
        AveragePass();

        void SetTextures(std::vector<Texture>);
        void render(RenderState& renderState, const Scene& scene) override;

    private:
        static const unsigned int MAX_SOURCES = 8;

        Shader shader;

        std::vector<Texture> textures;

        std::vector<int> units;
    };
}

#endif /* AVERAGE_PASS_H */

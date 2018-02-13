#pragma once
#ifndef COLOR_GRADING_PASS_H
#define COLOR_GRADING_PASS_H

#include "RenderPhase.h"

#include "Shader.h"

#include <memory>

namespace Flux
{
    class Texture;
    class Texture3D;

    class ColorGradingPass : public RenderPhase
    {
    public:
        ColorGradingPass();

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        Shader shader;

        Texture3D* lut;
    };
}

#endif /* COLOR_GRADING_PASS_H */

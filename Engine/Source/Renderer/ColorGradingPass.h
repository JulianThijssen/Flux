#pragma once

#include "RenderPhase.h"

#include "Shader.h"

#include <memory>

namespace Flux
{
    class Texture2D;
    class Texture3D;

    class ColorGradingPass : public RenderPhase
    {
    public:
        ColorGradingPass();

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        Shader shader;

        Texture3D lut;
    };
}

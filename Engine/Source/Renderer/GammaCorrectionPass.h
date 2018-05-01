#pragma once

#include "RenderPhase.h"

#include "Shader.h"

#include <memory>

namespace Flux
{
    class Texture2D;

    class GammaCorrectionPass : public RenderPhase
    {
    public:
        GammaCorrectionPass();

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        Shader shader;
    };
}

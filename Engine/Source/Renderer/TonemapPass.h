#pragma once

#include "RenderPhase.h"

#include <memory>

namespace Flux
{
    enum Tonemapper
    {
        REINHARD = 0,
        FILMIC = 1
    };

    class TonemapPass : public RenderPhase
    {
    public:
        TonemapPass();

        void setTonemapper(Tonemapper tonemapper);

        void setExposure(float exposure);

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        ShaderProgram shader;

        Tonemapper tonemapper = REINHARD;
        float exposure = 1.0f;
    };
}

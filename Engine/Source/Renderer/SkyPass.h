#pragma once

#include "RenderPhase.h"

#include <memory>

namespace Flux {
    class SkyPass : public RenderPhase
    {
    public:
        SkyPass();

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        ShaderProgram skyboxShader;
        ShaderProgram skysphereShader;
        ShaderProgram skyShader;
        ShaderProgram texShader;
    };
}

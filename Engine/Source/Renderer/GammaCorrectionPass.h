#pragma once
#ifndef GAMMA_CORRECTION_PASS_H
#define GAMMA_CORRECTION_PASS_H

#include "RenderPhase.h"

#include "Shader.h"

#include <memory>

namespace Flux
{
    class Texture;

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

#endif /* GAMMA_CORRECTION_PASS_H */

#pragma once
#ifndef FXAA_PASS_H
#define FXAA_PASS_H

#include "RenderPhase.h"

#include "Shader.h"
#include "Util/Size.h"

#include <memory>

namespace Flux
{
    class Texture;
    class Size;

    class FxaaPass : public RenderPhase
    {
    public:
        FxaaPass();

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        Shader shader;

        Size windowSize;
    };
}

#endif /* FXAA_PASS_H */

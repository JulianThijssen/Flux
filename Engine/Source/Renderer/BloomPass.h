#pragma once

#include "RenderPhase.h"
#include "AddPass.h"
#include "GaussianBlurPass.h"

#include "Shader.h"

#include <memory>

namespace Flux {
    class Texture2D;
    class Framebuffer;

    class BloomPass : public RenderPhase
    {
    public:
        BloomPass();

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        Shader shader;

        Framebuffer buffer;

        GaussianBlurPass blurPass;
        AddPass addPass;
    };
}

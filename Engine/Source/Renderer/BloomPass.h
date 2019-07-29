#pragma once

#include "RenderPhase.h"
#include "AddPass.h"
#include "GaussianBlurPass.h"

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
        ShaderProgram shader;

        Framebuffer buffer;

        GaussianBlurPass blurPass;
        AddPass addPass;
    };
}

#pragma once

#include "RenderPhase.h"
#include "AddPass.h"

#include "Shader.h"
#include "Util/Size.h"

#include <memory>

namespace Flux {
    class Texture2D;
    class Framebuffer;

    class GaussianBlurPass : public RenderPhase
    {
    public:
        GaussianBlurPass();

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        Shader shader;

        Size windowSize;
        std::vector<Framebuffer> blurBuffers;

        AddPass averagePass;
    };
}

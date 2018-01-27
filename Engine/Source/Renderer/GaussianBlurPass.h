#pragma once
#ifndef GAUSSIAN_BLUR_PASS_H
#define GAUSSIAN_BLUR_PASS_H

#include "RenderPhase.h"

#include "Shader.h"
#include "Util/Size.h"

#include <memory>

namespace Flux {
    class Texture;
    class Framebuffer;

    class AveragePass;

    class GaussianBlurPass : public RenderPhase
    {
    public:
        GaussianBlurPass();

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        Shader shader;

        const Framebuffer* target;

        Size windowSize;
        std::vector<Framebuffer*> blurBuffers;
        std::vector<Framebuffer*> blurBuffers2;

        AveragePass* averagePass;
    };
}

#endif /* GAUSSIAN_BLUR_PASS_H */

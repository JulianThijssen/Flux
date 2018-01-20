#pragma once
#ifndef COLOR_GRADING_PASS_H
#define COLOR_GRADING_PASS_H

#include "RenderPhase.h"

#include "Shader.h"

#include <memory>

namespace Flux
{
    class Texture;
    class Texture3D;
    class Framebuffer;

    class ColorGradingPass : public RenderPhase
    {
    public:
        ColorGradingPass();

        void SetTarget(const Framebuffer* target);

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        Shader shader;

        const Framebuffer* target;

        Texture3D* lut;
    };
}

#endif /* COLOR_GRADING_PASS_H */

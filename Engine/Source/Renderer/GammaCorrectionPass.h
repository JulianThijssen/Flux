#pragma once
#ifndef GAMMA_CORRECTION_PASS_H
#define GAMMA_CORRECTION_PASS_H

#include "RenderPhase.h"

#include "Shader.h"

#include <memory>

namespace Flux
{
    class Texture;
    class Framebuffer;

    class GammaCorrectionPass : public RenderPhase
    {
    public:
        GammaCorrectionPass();

        void SetSource(const Texture* source);
        void SetTarget(const Framebuffer* target);

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        Shader shader;

        const Texture* source;
        const Framebuffer* target;
    };
}

#endif /* GAMMA_CORRECTION_PASS_H */

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
    class Framebuffer;
    class Size;

    class FxaaPass : public RenderPhase
    {
    public:
        FxaaPass();

        void SetSource(const Texture* source);
        void SetTarget(const Framebuffer* target);
        void Resize(const Size& windowSize);

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        std::unique_ptr<Shader> shader;

        const Texture* source;
        const Framebuffer* target;

        Size windowSize;
    };
}

#endif /* FXAA_PASS_H */

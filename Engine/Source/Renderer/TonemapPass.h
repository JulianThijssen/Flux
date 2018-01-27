#pragma once
#ifndef TONEMAP_PASS_H
#define TONEMAP_PASS_H

#include "RenderPhase.h"

#include "Shader.h"

#include <memory>

namespace Flux
{
    class Texture;
    class Framebuffer;

    class TonemapPass : public RenderPhase
    {
    public:
        TonemapPass();

        void SetBloom(const Texture* bloom);

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        Shader shader;

        const Texture* source;
        const Texture* bloom;
    };
}

#endif /* TONEMAP_PASS_H */

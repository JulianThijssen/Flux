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

        void SetSource(const Texture* source);
        void SetBloom(const Texture* bloom);
        void SetTarget(const Framebuffer* target);

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        std::unique_ptr<Shader> shader;

        const Texture* source;
        const Texture* bloom;
        const Framebuffer* target;
    };
}

#endif /* TONEMAP_PASS_H */

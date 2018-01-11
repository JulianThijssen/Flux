#pragma once
#ifndef BLOOM_PASS_H
#define BLOOM_PASS_H

#include "RenderPhase.h"

#include "Shader.h"

#include <memory>

namespace Flux {
    class Texture;
    class Framebuffer;

    class BloomPass : public RenderPhase
    {
    public:
        BloomPass();

        void SetSource(const Texture* source);
        void SetTarget(const Framebuffer* target);

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        Shader shader;

        const Texture* source;
        const Framebuffer* target;
    };
}

#endif /* BLOOM_PASS_H */

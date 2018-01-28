#pragma once
#ifndef BLOOM_PASS_H
#define BLOOM_PASS_H

#include "RenderPhase.h"
#include "AddPass.h"

#include "Shader.h"

#include <memory>

namespace Flux {
    class Texture;
    class Framebuffer;

    class BloomPass : public RenderPhase
    {
    public:
        BloomPass();

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        Shader shader;

        std::vector<Framebuffer> buffers;

        AddPass addPass;
    };
}

#endif /* BLOOM_PASS_H */

#pragma once
#ifndef DIRECT_LIGHT_PASS_H
#define DIRECT_LIGHT_PASS_H

#include "RenderPhase.h"

#include "Shader.h"
#include "Renderer/GBuffer.h"

#include <memory>

namespace Flux
{
    class Texture;
    class Framebuffer;

    class DirectLightPass : public RenderPhase
    {
    public:
        DirectLightPass();

        void SetGBuffer(const GBuffer* gBuffer);
        void SetTarget(const Framebuffer* target);

        void render(const Scene& scene) override;

    private:
        std::unique_ptr<Shader> shader;

        const GBuffer* gBuffer;
        const Framebuffer* target;
    };
}

#endif /* DIRECT_LIGHT_PASS_H */

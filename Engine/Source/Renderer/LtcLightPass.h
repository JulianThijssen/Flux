#pragma once
#ifndef LTC_LIGHT_PASS_H
#define LTC_LIGHT_PASS_H

#include "RenderPhase.h"

#include "Shader.h"
#include "Renderer/GBuffer.h"

#include <memory>

namespace Flux
{
    class Texture;
    class Framebuffer;

    class LtcLightPass : public RenderPhase
    {
    public:
        LtcLightPass();

        void SetGBuffer(const GBuffer* gBuffer);
        void SetTarget(const Framebuffer* target);

        void render(const Scene& scene) override;

    private:
        std::unique_ptr<Shader> shader;

        const GBuffer* gBuffer;
        const Framebuffer* target;

        const Texture* ampTex;
        const Texture* aTex;
        const Texture* bTex;
        const Texture* cTex;
        const Texture* dTex;
    };
}

#endif /* LTC_LIGHT_PASS_H */

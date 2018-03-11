#pragma once
#ifndef DIRECT_LIGHT_PASS_H
#define DIRECT_LIGHT_PASS_H

#include "RenderPhase.h"

#include "AddPass.h"
#include "Shader.h"
#include "Renderer/GBuffer.h"

#include <memory>

namespace Flux
{
    class Texture2D;

    class DirectLightPass : public RenderPhase
    {
    public:
        DirectLightPass();

        void SetGBuffer(const GBuffer* gBuffer);

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        Shader shader;

        const GBuffer* gBuffer;

        const Texture2D ampTex;
        const Texture2D matTex;

        AddPass addPass;
    };
}

#endif /* DIRECT_LIGHT_PASS_H */

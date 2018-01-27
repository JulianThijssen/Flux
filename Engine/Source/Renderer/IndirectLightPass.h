#pragma once
#ifndef INDIRECT_LIGHT_PASS_H
#define INDIRECT_LIGHT_PASS_H

#include "RenderPhase.h"

#include "Shader.h"
#include "Renderer/GBuffer.h"
#include "Renderer/ImageBasedRendering.h"

#include <memory>

namespace Flux
{
    class Texture;
    class Framebuffer;

    class IndirectLightPass : public RenderPhase
    {
    public:
        IndirectLightPass(const Scene& scene);

        void SetGBuffer(const GBuffer* gBuffer);

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        Shader shader;

        const GBuffer* gBuffer;
        IblSceneInfo iblSceneInfo;

        bool sky = true;
    };
}

#endif /* INDIRECT_LIGHT_PASS_H */

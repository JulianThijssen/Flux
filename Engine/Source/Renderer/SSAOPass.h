#pragma once
#ifndef SSAO_PASS_H
#define SSAO_PASS_H

#include "RenderPhase.h"

#include "Shader.h"
#include "Renderer/GBuffer.h"
#include "Renderer/SSAORendering.h"

#include <memory>

namespace Flux
{
    class Texture;
    class Size;

    class SSAOPass : public RenderPhase
    {
    public:
        SSAOPass();

        void SetGBuffer(const GBuffer* gBuffer);
        void SetSsaoInfo(SsaoInfo* ssaoInfo);
        void SetWindowSize(const Size* size);

        void render(const Scene& scene) override;

    private:
        std::unique_ptr<Shader> ssaoShader;
        std::unique_ptr<Shader> blurShader;

        const GBuffer* gBuffer;
        SsaoInfo* ssaoInfo;
        const Size* windowSize;
    };
}

#endif /* SSAO_PASS_H */

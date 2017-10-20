#pragma
#ifndef SSAO_PASS_H
#define SSAO_PASS_H

#include <Engine/Source/RenderPhase.h>

#include <Engine/Source/Shader.h>
#include <Engine/Source/Renderer/GBuffer.h>
#include <Engine/Source/Renderer/SSAORendering.h>

#include <memory>

namespace Flux
{
    class Texture;

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

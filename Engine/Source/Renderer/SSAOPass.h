#pragma once
#ifndef SSAO_PASS_H
#define SSAO_PASS_H

#include "RenderPhase.h"
#include "MultiplyPass.h"

#include "Shader.h"
#include "Renderer/GBuffer.h"

#include <memory>

namespace Flux
{
    class Texture2D;
    class Size;

    class SSAOPass : public RenderPhase
    {
    public:
        SSAOPass();

        void generate();

        void SetGBuffer(const GBuffer* gBuffer);

        void Resize(const Size& windowSize) override;

        void render(RenderState& renderState, const Scene& scene) override;

    private:
        Shader ssaoShader;
        Shader blurShader;

        MultiplyPass multiplyPass;

        Size windowSize;

        const GBuffer* gBuffer;

        Framebuffer buffer;

        std::vector<Vector3f> kernel;
        std::vector<Vector3f> noise;
        Texture2D* noiseTexture;
    };
}

#endif /* SSAO_PASS_H */

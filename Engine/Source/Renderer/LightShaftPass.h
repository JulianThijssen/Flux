#pragma once

#include "RenderPhase.h"

#include "AddPass.h"
#include "Shader.h"
#include "Util/Size.h"

#include <memory>

namespace Flux
{
    class Framebuffer;
    class Texture2D;

    class LightShaftPass : public RenderPhase
    {
    public:
        LightShaftPass();

        virtual void Resize(const Size& windowSize) override;

        virtual void render(RenderState& renderState, const Scene& scene) override;

        void setExposure(float exposure);
        void setDensity(float density);
        void setDecay(float decay);

    private:
        Shader texShader;
        Shader shader;

        Size windowSize;

        Framebuffer buffer;
        Texture2D lightTex;

        float exposure;
        float density;
        float decay;

        AddPass addPass;
    };
}

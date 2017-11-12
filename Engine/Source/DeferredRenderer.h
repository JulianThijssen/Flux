#pragma once
#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H

#include "Renderer.h"
#include "Renderer/GBuffer.h"

#include "Cubemap.h"
#include "Renderer/ImageBasedRendering.h"
#include "Renderer/SSAORendering.h"

#include "Texture.h"

#include <memory>

namespace Flux {
    class AveragePass;
    class MultiplyPass;
    class SSAOPass;
    class SkyPass;
    class BloomPass;
    class GaussianBlurPass;
    class TonemapPass;
    class DirectLightPass;
    class GammaCorrectionPass;
    class FxaaPass;
    class ColorGradingPass;
    class FogPass;
    class LtcLightPass;

    class Size;

    class DeferredRenderer : public Renderer {
    public:
        DeferredRenderer() :
        averagePass()
        { }

        virtual bool create(const Scene& scene, const Size windowSize);
        virtual void onResize(const Size windowSize);
        virtual void update(const Scene& scene);
        virtual void renderScene(const Scene& scene);
        virtual void renderMesh(const Scene& scene, Entity* e);

    private:
        void createBackBuffers(const unsigned int width, const unsigned int height);
        void createShadowMaps(const Scene& scene);
        void globalIllumination(const Scene& scene);
        void directLighting(const Scene& scene);
        void ssao(const Scene& scene);
        void multiply(const Scene& scene);
        void applyPostprocess(const Scene& scene);
        void fog(const Scene& scene);
        void bloom(const Scene& scene);
        void blur(const Scene& scene);
        void tonemap(const Scene& scene);
        void gammaCorrection(const Scene& scene);
        void antiAliasing(const Scene& scene);
        void colorGrading(const Scene& scene);
        void renderDepth(const Scene& scene);
        void renderShadowMaps(const Scene& scene);
        void renderFramebuffer(const Framebuffer& framebuffer);

        GBuffer gBuffer;
        std::unique_ptr<Framebuffer> shadowBuffer;
        IblSceneInfo iblSceneInfo;

        AveragePass* averagePass;
        MultiplyPass* multiplyPass;
        SSAOPass* ssaoPass;
        SkyPass* skyPass;
        BloomPass* bloomPass;
        GaussianBlurPass* gaussianBlurPass;
        TonemapPass* tonemapPass;
        DirectLightPass* directLightPass;
        GammaCorrectionPass* gammaCorrectionPass;
        FxaaPass* fxaaPass;
        ColorGradingPass* colorGradingPass;
        FogPass* fogPass;
        LtcLightPass* areaLightPass;

        SsaoInfo ssaoInfo;
    };
}

#endif /* DEFERRED_RENDERER_H */

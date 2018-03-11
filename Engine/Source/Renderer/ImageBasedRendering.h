#pragma once
#ifndef IMAGE_BASED_RENDERING_H
#define IMAGE_BASED_RENDERING_H

#include "Skybox.h"
#include "Texture.h"

namespace Flux
{
    class IrradianceMap : public Cubemap
    {
    public:
        IrradianceMap(const Texture* environmentTex)
            :
            envTex(environmentTex),
            envMap(nullptr),
            skybox(false)
        { }
        IrradianceMap(const Skybox* environmentMap)
            :
            envMap(environmentMap),
            envTex(nullptr),
            skybox(true)
        { }

        void generate(const uint resolution);
    private:
        const Cubemap* envMap;
        const Texture* envTex;
        bool skybox;
    };

    class PrefilterEnvmap : public Cubemap
    {
    public:
        PrefilterEnvmap(const Texture2D* environmentTex)
            :
            envTex(environmentTex),
            envMap(nullptr),
            skybox(false)
        { }
        PrefilterEnvmap(const Skybox* environmentMap)
            :
            envMap(environmentMap),
            envTex(nullptr),
            skybox(true)
        { }

        void generate(const uint resolution);
    private:
        const Cubemap* envMap;
        const Texture2D* envTex;
        bool skybox;
    };

    class ScaleBiasTexture : public Texture2D
    {
    public:
        ScaleBiasTexture();

        void generate();
    };

    class IblSceneInfo
    {
    public:
        void PrecomputeEnvironmentData(const Texture2D& environmentTex);
        void PrecomputeEnvironmentData(const Skybox& skybox);

        IrradianceMap* irradianceMap;
        PrefilterEnvmap* prefilterEnvmap;
        ScaleBiasTexture* scaleBiasTexture;
    };
}

#endif /* IMAGE_BASED_RENDERING_H */

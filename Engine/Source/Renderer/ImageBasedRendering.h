#pragma once
#ifndef IMAGE_BASED_RENDERING_H
#define IMAGE_BASED_RENDERING_H

#include <Engine/Source/Skybox.h>
#include <Engine/Source/Cubemap.h>
#include <Engine/Source/Texture.h>

namespace Flux
{
    class IrradianceMap : public Cubemap
    {
    public:
        IrradianceMap(const Skybox& environmentMap) : envMap(environmentMap) {}

        void generate(const unsigned int textureSize);
    private:
        const Cubemap& envMap;
    };

    class PrefilterEnvmap : public Cubemap
    {
    public:
        PrefilterEnvmap(const Skybox& environmentMap) : envMap(environmentMap) {}

        void generate();
    private:
        const Cubemap& envMap;
    };

    class ScaleBiasTexture : public Texture
    {
    public:
        ScaleBiasTexture();

        void generate();
    };

    class IblSceneInfo
    {
    public:
        void PrecomputeEnvironmentData(const Skybox& skybox);

        IrradianceMap* irradianceMap;
        PrefilterEnvmap* prefilterEnvmap;
        ScaleBiasTexture* scaleBiasTexture;
    };
}

#endif /* IMAGE_BASED_RENDERING_H */

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
        IrradianceMap(const Texture* environmentTex) : envTex(environmentTex), envMap(nullptr), skybox(false) {}
        IrradianceMap(const Skybox* environmentMap) : envMap(environmentMap), envTex(nullptr), skybox(true) {}

        void generate(const unsigned int textureSize);
    private:
        const Cubemap* envMap;
        const Texture* envTex;
        bool skybox;
    };

    class PrefilterEnvmap : public Cubemap
    {
    public:
        PrefilterEnvmap(const Texture* environmentTex) : envTex(environmentTex), envMap(nullptr), skybox(false) {}
        PrefilterEnvmap(const Skybox* environmentMap) : envMap(environmentMap), envTex(nullptr), skybox(true) {}

        void generate();
    private:
        const Cubemap* envMap;
        const Texture* envTex;
        bool skybox;
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
        void PrecomputeEnvironmentData(const Texture& environmentTex);
        void PrecomputeEnvironmentData(const Skybox& skybox);

        IrradianceMap* irradianceMap;
        PrefilterEnvmap* prefilterEnvmap;
        ScaleBiasTexture* scaleBiasTexture;
    };

    class ImageBasedRendering
    {
    public:
        void create();
        void render();
    };
}

#endif /* IMAGE_BASED_RENDERING_H */

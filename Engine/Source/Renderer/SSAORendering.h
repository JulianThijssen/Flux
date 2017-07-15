#pragma once
#ifndef SSAO_RENDERING_H
#define SSAO_RENDERING_H

#include <Engine/Source/Texture.h>

#include <vector>

namespace Flux
{
    class Vector3f;

    class SsaoInfo
    {
    public:
        SsaoInfo() : noiseTexture(0) {}

        void generate(int samples, int noiseSamples);

        std::vector<Vector3f> kernel;
        std::vector<Vector3f> noise;
        Texture* noiseTexture;
    };
}

#endif /* SSAO_RENDERING_H */

#include "SSAORendering.h"

#include <Engine/Source/Vector3f.h>
#include <Engine/Source/TextureLoader.h>

#include <glad/glad.h>
#include <cstdlib>
#include <random>
#include <iostream>

float random(float low, float high)
{
    return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
}

Flux::Vector3f uniformHemisphereSample()
{
    std::default_random_engine generator(std::random_device{}());
    std::normal_distribution<float> distribution(0.0, 1.0);

    float x = distribution(generator);
    float y = distribution(generator);
    float z = distribution(generator);

    Flux::Vector3f point(x, y, z);
    point.normalise();

    return (Flux::dot(point, Flux::Vector3f(0, 0, 1)) > 0) ? point : -point;
}

Flux::Vector3f ssaoKernel(int i, int numSamples)
{
    Flux::Vector3f point = uniformHemisphereSample();
    float dist = random(0, 1);
    point *= dist;

    float scale = (float)i / numSamples;
    scale = (1 - scale*scale) * 0.1f + scale*scale;

    point *= scale;
    return point;
}

namespace Flux
{
    void SsaoInfo::generate(int samples, int noiseSamples)
    {
        // Generate a hemispherical kernel
        srand(0);
        kernel.reserve(samples);
        for (int i = 0; i < samples; i++) {
            Vector3f v = ssaoKernel(i, samples);
            kernel.push_back(v);
        }

        // Generate a noise texture
        noise.reserve(noiseSamples*noiseSamples);
        for (int i = 0; i < noiseSamples*noiseSamples; i++) {
            Vector3f v(random(-1, 1), random(-1, 1), 0.0f);
            v.normalise();
            v = v * 0.5 + Vector3f(0.5, 0.5, 0.5);
            noise.push_back(v);
        }

        GLuint noiseHandle;
        glGenTextures(1, &noiseHandle);
        glBindTexture(GL_TEXTURE_2D, noiseHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, noiseSamples, noiseSamples, 0, GL_RGB, GL_FLOAT, noise.data());
        noiseTexture = new Texture(noiseHandle);
    }

    //void SsaoInfo::bindNoiseTexture()
    //{
    //    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    //}
}

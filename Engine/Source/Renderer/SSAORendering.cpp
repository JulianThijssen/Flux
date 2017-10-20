#include "SSAORendering.h"

#include <Engine/Source/Vector3f.h>
#include <Engine/Source/TextureLoader.h>

#include <Engine/Source/Framebuffer.h>

#include <glad/glad.h>
#include <cstdlib>
#include <random>
#include <iostream>

const unsigned int NUM_SAMPLES = 13;
const unsigned int NOISE_SIZE = 4;


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

    float scale = (float)i / numSamples;
    scale = (1 - scale*scale) * 0.1f + scale*scale;
    point *= scale;

    return point;
}

namespace Flux
{
    Framebuffer* SsaoInfo::getCurrentBuffer() const
    {
        return buffers[bufferIndex];
    }

    void SsaoInfo::switchBuffers()
    {
        bufferIndex = bufferIndex == 0 ? 1 : 0;
    }

    void SsaoInfo::createBuffers(unsigned int width, unsigned int height)
    {
        // Generate half sized framebuffers for low-resolution SSAO rendering
        buffers.resize(2);
        for (int i = 0; i < 2; i++) {
            buffers[i] = new Framebuffer(width / 2, height / 2);
            buffers[i]->bind();
            buffers[i]->addColorTexture(0, TextureLoader::createEmpty(width / 2, height / 2, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, Sampling::NEAREST, false));
            buffers[i]->validate();
            buffers[i]->release();
        }
    }

    void SsaoInfo::generate()
    {
        // Generate a hemispherical kernel
        srand(0);
        kernel.resize(NUM_SAMPLES);

        kernel[0].set(0.0f, -0.92388f, 0.38268f);
        kernel[1].set(0.92388f, 0.0f, 0.38268f);
        kernel[2].set(0.0f, 0.92388f, 0.38268f);
        kernel[3].set(-0.92388f, 0.0f, 0.2f);
        kernel[4].set(0.5f, -0.5f, 0.70711f);
        kernel[5].set(0.5f, 0.5f, 0.70711f);
        kernel[6].set(-0.5f, 0.5f, 0.70711f);
        kernel[7].set(-0.5f, -0.5f, 0.70711f);
        kernel[8].set(0.0f, -0.38268f, 0.92388f);
        kernel[9].set(0.38268f, 0.0f, 0.92388f);
        kernel[10].set(0.0f, 0.38268f, 0.92388f);
        kernel[11].set(-0.38268f, 0.0f, 0.92388f);
        kernel[12].set(0.0f, 0.0f, 1.0f);

        for (int i = 0; i < NUM_SAMPLES; i++) {
            float scale = (float)i / NUM_SAMPLES;
            scale = (1 - scale*scale) * 0.1f + scale*scale;

            kernel[i] *= scale;
        }

        // Generate a noise texture
        noise.reserve(NOISE_SIZE*NOISE_SIZE);
        for (int i = 0; i < NOISE_SIZE*NOISE_SIZE; i++) {
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, NOISE_SIZE, NOISE_SIZE, 0, GL_RGB, GL_FLOAT, noise.data());
        noiseTexture = new Texture(noiseHandle, NOISE_SIZE, NOISE_SIZE);
    }
}

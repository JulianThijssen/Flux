#include "Renderer/ImageBasedRendering.h"

#include "Framebuffer.h"
#include "Shader.h"
#include "Renderer/RenderState.h"

#include <glad/glad.h>
#include <iostream>

namespace Flux
{
    void IrradianceMap::generate(const unsigned int textureSize)
    {
        Shader shader;
        shader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/Irradiance.frag");

        Framebuffer framebuffer;
        framebuffer.create();
        framebuffer.bind();
        framebuffer.addDrawBuffer(GL_COLOR_ATTACHMENT0);

        glClearColor(1, 0, 0, 1);

        shader.bind();

        if (skybox) {
            createEmpty(textureSize, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, REPEAT, false);

            envMap->bind(0);
            shader.uniform1i("EnvMap", 0);
        }
        else {
            createEmpty(textureSize, GL_RGBA16F, GL_RGBA, GL_FLOAT, REPEAT, false);

            envTex->bind(0);
            shader.uniform1i("EnvTex", 0);
        }

        shader.uniform1i("Skybox", skybox);
        // Should be resolution of environment map for perfect accuracy, but this is good enough
        shader.uniform1i("textureSize", textureSize * 4);

        glViewport(0, 0, textureSize, textureSize);

        for (int i = 0; i < 6; i++)
        {
            shader.uniform1i("Face", i);
            framebuffer.setCubemap(getHandle(), i, 0);
            framebuffer.validate();

            glBindVertexArray(RenderState::quadVao);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        framebuffer.release();
    }

    void PrefilterEnvmap::generate()
    {
        Shader shader;
        shader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/PrefilterEnvmap.frag");

        // Should be resolution of environment map for perfect accuracy, but this is good enough
        const unsigned int resolution = 512;

        Framebuffer framebuffer;
        framebuffer.create();
        framebuffer.bind();
        framebuffer.addDrawBuffer(GL_COLOR_ATTACHMENT0);

        glClearColor(1, 0, 0, 1);

        shader.bind();

        if (skybox) {
            createEmpty(resolution, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, REPEAT, true);

            envMap->bind(0);
            shader.uniform1i("EnvMap", 0);
        }
        else {
            createEmpty(resolution, GL_RGBA16F, GL_RGBA, GL_FLOAT, REPEAT, true);

            envTex->bind(0);
            shader.uniform1i("EnvTex", 0);
        }

        shader.uniform1i("Skybox", skybox);

        const unsigned int MIP_MAP_LEVELS = 6;
        for (int level = 0; level < MIP_MAP_LEVELS; level++)
        {
            unsigned int mipmapSize = resolution >> level;
            glViewport(0, 0, mipmapSize, mipmapSize);
            float Roughness = (float)level / (MIP_MAP_LEVELS - 1);
            std::cout << "Roughness: " << Roughness << std::endl;
            shader.uniform1f("Roughness", Roughness);

            for (int i = 0; i < 6; i++)
            {
                shader.uniform1i("Face", i);
                framebuffer.setCubemap(getHandle(), i, level);
                framebuffer.validate();

                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        framebuffer.release();
    }

    ScaleBiasTexture::ScaleBiasTexture()
        : Texture(0, 256, 256)
    {
        glGenTextures(1, &handle);

        glBindTexture(GL_TEXTURE_2D, handle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getWidth(), getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void ScaleBiasTexture::generate()
    {
        Shader shader;
        shader.loadFromFile("res/Shaders/Quad.vert", "res/Shaders/BRDFintegration.frag");

        Framebuffer framebuffer;
        framebuffer.create();
        framebuffer.bind();
        framebuffer.addDrawBuffer(GL_COLOR_ATTACHMENT0);

        glClearColor(1, 0, 0, 1);

        shader.bind();

        glViewport(0, 0, 256, 256);

        framebuffer.setTexture(GL_COLOR_ATTACHMENT0, *this);
        framebuffer.validate();

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        framebuffer.release();
    }

    void IblSceneInfo::PrecomputeEnvironmentData(const Texture& environmentTex) {
        irradianceMap = new IrradianceMap(&environmentTex);
        irradianceMap->generate(32);

        prefilterEnvmap = new PrefilterEnvmap(&environmentTex);
        prefilterEnvmap->generate();

        scaleBiasTexture = new ScaleBiasTexture();
        scaleBiasTexture->generate();
    }

    void IblSceneInfo::PrecomputeEnvironmentData(const Skybox& skybox) {
        irradianceMap = new IrradianceMap(&skybox);
        irradianceMap->generate(32);

        prefilterEnvmap = new PrefilterEnvmap(&skybox);
        prefilterEnvmap->generate();

        scaleBiasTexture = new ScaleBiasTexture();
        scaleBiasTexture->generate();
    }
}

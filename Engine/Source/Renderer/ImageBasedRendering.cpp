#include "ImageBasedRendering.h"

#include <Engine/Source/Framebuffer.h>
#include <Engine/Source/Shader.h>
#include <Engine/Source/ShaderLoader.h>

#include <Engine/Source/Exceptions/ShaderCompilationException.h>
#include <Engine/Source/Exceptions/ShaderLinkException.h>

#include <glad/glad.h>
#include <iostream>

namespace Flux
{
    void IrradianceMap::generate(const unsigned int textureSize)
    {
        Shader* shader = Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/Irradiance.frag");

        Framebuffer framebuffer;
        framebuffer.bind();
        framebuffer.setDrawBuffer(GL_COLOR_ATTACHMENT0);

        glClearColor(1, 0, 0, 1);

        shader->bind();

        createEmpty(textureSize, false);

        envMap.bind(0);
        shader->uniform1i("EnvMap", 0);

        shader->uniform1i("textureSize", envMap.getResolution());
        glViewport(0, 0, textureSize, textureSize);

        for (int i = 0; i < 6; i++)
        {
            shader->uniform1i("Face", i);
            framebuffer.setCubemap(getHandle(), i, 0);
            framebuffer.validate();

            glClear(GL_COLOR_BUFFER_BIT);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        framebuffer.release();
    }

    void PrefilterEnvmap::generate()
    {
        Shader* shader;
        try
        {
            shader = Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/PrefilterEnvmap.frag");
        }
        catch (const ShaderCompilationException& e)
        {
            Log::error(e.what());
        }
        catch (const ShaderLinkException& e)
        {
            Log::error(e.what());
        }

        Framebuffer framebuffer;
        framebuffer.bind();
        framebuffer.setDrawBuffer(GL_COLOR_ATTACHMENT0);

        glClearColor(1, 0, 0, 1);

        shader->bind();

        createEmpty(envMap.getResolution(), true);

        envMap.bind(0);
        shader->uniform1i("EnvMap", 0);

        const unsigned int MIP_MAP_LEVELS = 6;
        for (int level = 0; level < MIP_MAP_LEVELS; level++)
        {
            unsigned int mipmapSize = envMap.getResolution() >> level;
            glViewport(0, 0, mipmapSize, mipmapSize);
            float Roughness = (float)level / (MIP_MAP_LEVELS - 1);
            shader->uniform1f("Roughness", Roughness);

            for (int i = 0; i < 6; i++)
            {
                shader->uniform1i("Face", i);
                framebuffer.setCubemap(getHandle(), i, level);
                framebuffer.validate();
                
                glClear(GL_COLOR_BUFFER_BIT);

                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        framebuffer.release();
    }

    ScaleBiasTexture::ScaleBiasTexture()
        : Texture(0)
    {
        glGenTextures(1, &handle);

        glBindTexture(GL_TEXTURE_2D, handle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void ScaleBiasTexture::generate()
    {
        Shader* shader;
        try
        {
            shader = Shader::fromFile("res/Shaders/Quad.vert", "res/Shaders/BRDFintegration.frag");
        }
        catch (const ShaderCompilationException& e)
        {
            Log::error(e.what());
        }
        catch (const ShaderLinkException& e)
        {
            Log::error(e.what());
        }

        Framebuffer framebuffer;
        framebuffer.bind();
        framebuffer.setDrawBuffer(GL_COLOR_ATTACHMENT0);

        glClearColor(1, 0, 0, 1);

        shader->bind();

        glViewport(0, 0, 256, 256);

        framebuffer.setTexture(GL_COLOR_ATTACHMENT0, handle);
        framebuffer.validate();

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        framebuffer.release();
    }

    void IblSceneInfo::PrecomputeEnvironmentData(const Skybox& skybox) {
        irradianceMap = new IrradianceMap(skybox);
        irradianceMap->generate(32);

        prefilterEnvmap = new PrefilterEnvmap(skybox);
        prefilterEnvmap->generate();

        scaleBiasTexture = new ScaleBiasTexture();
        scaleBiasTexture->generate();
    }
}

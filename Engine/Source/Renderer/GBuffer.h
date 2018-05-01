#pragma once

#include "Framebuffer.h"
#include "Texture.h"
#include "TextureUnit.h"

#include <memory>

namespace Flux {
    struct GBuffer {
    public:
        void create(const unsigned int width, const unsigned int height) {
            albedoTex = createAlbedoTex(width, height);
            normalTex = createNormalTex(width, height);
            positionTex = createPositionTex(width, height);
            emissionTex = createEmissionTex(width, height);
            depthTex = createDepthTex(width, height);

            buffer.create();
            buffer.bind();
            buffer.addColorTexture(0, albedoTex);
            buffer.addColorTexture(1, normalTex);
            buffer.addColorTexture(2, positionTex);
            buffer.addColorTexture(3, emissionTex);
            buffer.addDepthStencilTexture(depthTex);
            buffer.validate();
            buffer.release();
        }

        void bind() {
            buffer.bind();
        }

        Texture2D albedoTex;
        Texture2D normalTex;
        Texture2D positionTex;
        Texture2D emissionTex;
        Texture2D depthTex;

        Framebuffer buffer;

    private:
        Texture2D createAlbedoTex(const uint width, const uint height)
        {
            Texture2D albedoTex;
            albedoTex.create();
            albedoTex.bind(TextureUnit::TEXTURE0);
            albedoTex.setData(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            albedoTex.setWrapping(CLAMP, CLAMP);
            albedoTex.setSampling(NEAREST, NEAREST);
            return albedoTex;
        }

        Texture2D createNormalTex(const uint width, const uint height)
        {
            Texture2D normalTex;
            normalTex.create();
            normalTex.bind(TextureUnit::TEXTURE0);
            normalTex.setData(width, height, GL_RGBA16, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            normalTex.setWrapping(CLAMP, CLAMP);
            normalTex.setSampling(NEAREST, NEAREST);
            return normalTex;
        }

        Texture2D createPositionTex(const uint width, const uint height)
        {
            Texture2D positionTex;
            positionTex.create();
            positionTex.bind(TextureUnit::TEXTURE0);
            positionTex.setData(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, nullptr);
            positionTex.setWrapping(CLAMP, CLAMP);
            positionTex.setSampling(NEAREST, NEAREST);
            return positionTex;
        }

        Texture2D createEmissionTex(const uint width, const uint height)
        {
            Texture2D emissionTex;
            emissionTex.create();
            emissionTex.bind(TextureUnit::TEXTURE0);
            emissionTex.setData(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT, nullptr);
            emissionTex.setWrapping(CLAMP, CLAMP);
            emissionTex.setSampling(NEAREST, NEAREST);
            return emissionTex;
        }

        Texture2D createDepthTex(const uint width, const uint height)
        {
            Texture2D depthTex;
            depthTex.create();
            depthTex.bind(TextureUnit::TEXTURE0);
            depthTex.setData(width, height, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
            depthTex.setWrapping(CLAMP, CLAMP);
            depthTex.setSampling(NEAREST, NEAREST);
            return depthTex;
        }
    };
}

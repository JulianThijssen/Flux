#pragma once
#ifndef GBUFFER_H
#define GBUFFER_H

#include "Framebuffer.h"
#include "Texture.h"

#include <memory>

namespace Flux {
    struct GBuffer {
        void create(const unsigned int width, const unsigned int height) {
            albedoTex = TextureLoader::create(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, CLAMP);
            normalTex = TextureLoader::create(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, CLAMP);
            positionTex = TextureLoader::create(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, CLAMP);
            depthTex = TextureLoader::create(width, height, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, CLAMP);

            buffer.create();
            buffer.bind();
            buffer.addColorTexture(0, albedoTex);
            buffer.addColorTexture(1, normalTex);
            buffer.addColorTexture(2, positionTex);
            buffer.addDrawBuffer(GL_COLOR_ATTACHMENT0);
            buffer.addDrawBuffer(GL_COLOR_ATTACHMENT1);
            buffer.addDrawBuffer(GL_COLOR_ATTACHMENT2);
            buffer.addDepthTexture(depthTex);
            buffer.validate();
            buffer.release();
        }

        void bind() {
            buffer.bind();
        }

        Texture2D* albedoTex;
        Texture2D* normalTex;
        Texture2D* positionTex;
        Texture2D* depthTex;

        Framebuffer buffer;
    };
}

#endif /* GBUFFER_H */

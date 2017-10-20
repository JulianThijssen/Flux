#pragma once
#ifndef GBUFFER_H
#define GBUFFER_H

#include <Engine/Source/Framebuffer.h>
#include <Engine/Source/Texture.h>

#include <memory>

namespace Flux {
    struct GBuffer {
        void create(const unsigned int width, const unsigned int height) {
            albedoTex = TextureLoader::createEmpty(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, Sampling::NEAREST, false);
            normalTex = TextureLoader::createEmpty(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, Sampling::NEAREST, false);
            positionTex = TextureLoader::createEmpty(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, Sampling::NEAREST, false);
            depthTex = TextureLoader::createEmpty(width, height, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, Sampling::NEAREST, false);

            buffer = std::make_unique<Framebuffer>(width, height);
            buffer->bind();
            buffer->addColorTexture(0, albedoTex);
            buffer->addColorTexture(1, normalTex);
            buffer->addColorTexture(2, positionTex);
            buffer->addDrawBuffer(GL_COLOR_ATTACHMENT0);
            buffer->addDrawBuffer(GL_COLOR_ATTACHMENT1);
            buffer->addDrawBuffer(GL_COLOR_ATTACHMENT2);
            buffer->addDepthTexture(depthTex);
            buffer->validate();
            buffer->release();
        }

        void bind() {
            buffer->bind();
        }

        Texture* albedoTex;
        Texture* normalTex;
        Texture* positionTex;
        Texture* depthTex;

        std::unique_ptr<Framebuffer> buffer;
    };
}

#endif /* GBUFFER_H */

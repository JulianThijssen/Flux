#pragma once
#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include "Framebuffer.h"
#include "Texture.h"
#include "TextureLoader.h"

#include <glad/glad.h>

namespace Flux {
    class RenderBuffer : public Framebuffer {
    public:
        RenderBuffer(const unsigned int width, const unsigned int height)
        : width(width)
        , height(height)
        , Framebuffer()
        { }

        void addColorTexture(int colorAttachment, Texture* texture) {
            colorTexture = texture;
            GLint attachment = GL_COLOR_ATTACHMENT0 + colorAttachment;
            setTexture(attachment, *colorTexture);
            setDrawBuffer(attachment);
        }

        void addDepthTexture() {
            depthTexture = TextureLoader::createEmpty(width, height, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, Sampling::NEAREST);
            setTexture(GL_DEPTH_ATTACHMENT, *depthTexture);
        }

        const Texture& getColorTexture() {
            return *colorTexture;
        }

    private:
        unsigned int width;
        unsigned int height;

        Texture* colorTexture;
        Texture* depthTexture;
    };
}

#endif /* RENDERBUFFER_H */

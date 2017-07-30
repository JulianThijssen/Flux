#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

typedef unsigned int uint;

namespace Flux {
    enum Sampling { NEAREST, LINEAR };

    class Texture {
    public:
        Texture(GLuint handle, uint width, uint height)
        :
            handle(handle),
            width(width),
            height(height)
        { }

        void bind(const uint textureUnit) const {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, handle);
        }

        void release() const {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        uint getWidth() const {
            return width;
        }

        uint getHeight() const {
            return height;
        }

        GLuint handle;

        uint width, height;
    };
}

#endif /* TEXTURE_H */

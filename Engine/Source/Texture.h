#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

namespace Flux {
    enum Sampling { NEAREST, LINEAR };

    class Texture {
    public:
        Texture(GLuint handle) : handle(handle) { }

        void bind(const unsigned int textureUnit) const {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, handle);
        }

        void release() const {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        GLuint handle;
    };
}

#endif /* TEXTURE_H */

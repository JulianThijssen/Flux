#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

namespace Flux {
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
    protected:
        GLuint handle;
    };
}

#endif /* TEXTURE_H */

#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

namespace Flux {
    class Texture {
    public:
        Texture(GLuint handle) : handle(handle) { }

        void bind() {
            glBindTexture(GL_TEXTURE_2D, handle);
        }

        void release() {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    private:
        GLuint handle;
    };
}

#endif /* TEXTURE_H */

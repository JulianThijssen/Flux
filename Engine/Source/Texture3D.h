#pragma once
#ifndef TEXTURE_3D_H
#define TEXTURE_3D_H

#include <glad/glad.h>

typedef unsigned int uint;

namespace Flux {
    class Texture3D {
    public:
        Texture3D(GLuint handle, uint width, uint height, uint depth)
        :
            handle(handle),
            width(width),
            height(height),
            depth(depth)
        { }

        void bind(const uint textureUnit) const {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_3D, handle);
        }

        void release() const {
            glBindTexture(GL_TEXTURE_3D, 0);
        }

        uint getWidth() const {
            return width;
        }

        uint getHeight() const {
            return height;
        }

        uint getDepth() const {
            return depth;
        }

        GLuint handle;

        uint width, height, depth;
    };
}

#endif /* TEXTURE_3D_H */

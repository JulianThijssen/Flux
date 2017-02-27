#pragma once
#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "Texture.h"

namespace Flux {
    class Path;

    class TextureLoader {
    public:
        static Texture* loadTexture(Path path);
        static Texture* create(const int width, const int height, const unsigned char* data, Sampling sampling);
        static Texture* createEmpty(const int width, const int height, GLint internalFormat, GLenum format, GLenum type, Sampling sampling);
    };
}

#endif /* TEXTURE_LOADER_H */

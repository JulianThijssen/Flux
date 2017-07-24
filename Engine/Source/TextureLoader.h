#pragma once
#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "Texture.h"

namespace Flux {
    class Path;

    class TextureLoader {
    public:
        static Texture* loadTexture(Path path);
        static Texture* loadTextureGreyscale(Path path);
        static Texture* loadColorAndAlpha(Path color, Path alpha, Sampling sampling);
        static Texture* loadTextureHDR(Path path);
        static Texture* create(const int width, const int height, const int bpp, const unsigned char* data, Sampling sampling);
        static Texture* createHDR(const int width, const int height, const float* data, Sampling sampling);
        static Texture* createEmpty(const int width, const int height, GLint internalFormat, GLenum format, GLenum type, Sampling sampling, bool mipmaps);
        static Texture* createShadowMap(const int width, const int height);
    };
}

#endif /* TEXTURE_LOADER_H */

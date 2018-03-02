#pragma once
#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "Texture.h"

enum TextureType {
    COLOR, GREYSCALE, HDR
};

enum Isotropy {
    ANISOTROPIC, ISOTROPIC
};

enum Sampling {
    NONE, NEAREST, LINEAR
};

enum Wrapping {
    CLAMP, REPEAT
};

namespace Flux {
    class Path;

    struct SamplingConfig {
        Sampling minFilter;
        Sampling magFilter;
        Sampling mipFilter;

        SamplingConfig(Sampling minFilter, Sampling magFilter, Sampling mipFilter)
            : minFilter(minFilter), magFilter(magFilter), mipFilter(mipFilter) { }
    };

    class TextureLoader {
    public:
        static Texture2D* loadTexture(Path path, TextureType type, Wrapping wrapping, SamplingConfig sampling);
        static Texture2D* loadColorAndAlpha(Path color, Path alpha, Wrapping wrapping, SamplingConfig sampling);
        static Texture3D* loadTexture3D(Path path);
        static Texture2D* create(const int width, const int height, GLint internalFormat, GLenum format, GLenum type, Wrapping wrapping, SamplingConfig sampling = SamplingConfig(NEAREST, NEAREST, NONE), const void* data = nullptr, Isotropy isotropy = ISOTROPIC);
        static Texture2D* createShadowMap(const int width, const int height);
        static Texture3D* create3DTexture(const int width, const int height, const int depth, const int bpp, const unsigned char* data, Sampling sampling);

    private:
        static bool loadTextureFromFile(Path path, int& width, int& height, TextureType type, void** data);
    };
}

#endif /* TEXTURE_LOADER_H */

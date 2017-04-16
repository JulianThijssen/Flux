#include "TextureLoader.h"

#include "Path.h"
#include "Log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>

namespace Flux {
    Texture* TextureLoader::loadTexture(Path path) {
        int width, height, bpp;

        unsigned char* image = stbi_load(path.str().c_str(), &width, &height, &bpp, STBI_rgb_alpha);
        
        if (!image) {
            Log::error("Failed to load image at: " + path.str());
        }

        Texture* texture = create(width, height, image, Sampling::LINEAR);

        return texture;
    }

    Texture* TextureLoader::loadTextureHDR(Path path) {
        int width, height, bpp;

        float *data = stbi_loadf(path.str().c_str(), &width, &height, &bpp, STBI_rgb_alpha);

        if (!data) {
            Log::error("Failed to load image at: " + path.str());
        }

        Texture* texture = createHDR(width, height, data, Sampling::NEAREST);

        return texture;
    }

    Texture* TextureLoader::create(const int width, const int height, const unsigned char* data, Sampling sampling) {
        GLuint handle;
        glGenTextures(1, &handle);

        glBindTexture(GL_TEXTURE_2D, handle);

        if (sampling == Sampling::NEAREST) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        }
        else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        return new Texture(handle);
    }

    Texture* TextureLoader::createHDR(const int width, const int height, const float* data, Sampling sampling) {
        GLuint handle;
        glGenTextures(1, &handle);

        glBindTexture(GL_TEXTURE_2D, handle);

        if (sampling == Sampling::NEAREST) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        }
        else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        return new Texture(handle);
    }

    Texture* TextureLoader::createEmpty(const int width, const int height, GLint internalFormat, GLenum format, GLenum type, Sampling sampling) {
        GLuint handle;
        glGenTextures(1, &handle);

        glBindTexture(GL_TEXTURE_2D, handle);

        if (sampling == Sampling::NEAREST) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
        else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, 0);

        glBindTexture(GL_TEXTURE_2D, 0);

        return new Texture(handle);
    }
}

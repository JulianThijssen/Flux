#pragma once
#ifndef CUBEMAP_H
#define CUBEMAP_H

#include "String.h"

#include "stb_image.h"

#include <glad/glad.h>
#include <cassert>

namespace Flux {
    class Cubemap {
    public:
        Cubemap() : resolution(DEFAULT_RESOLUTION) { }

        void create(char* const paths[6], bool mipmaps) {
            glGenTextures(1, &handle);
            bind();

            for (int i = 0; i < 6; i++) {
                int width, height, bpp;
                unsigned char* data = stbi_load(paths[i], &width, &height, &bpp, STBI_rgb_alpha);
                assert(width == height);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                resolution = width;
            }

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            if (mipmaps) {
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            }
            else {
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            }
            
            release();
        }

        void createEmpty(const unsigned int resolution, GLint internalFormat, GLenum format, GLenum type, bool mipmaps) {
            this->resolution = resolution;

            glGenTextures(1, &handle);
            bind();

            for (int i = 0; i < 6; i++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, resolution, resolution, 0, format, type, 0);
            }

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            if (mipmaps) {
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 5);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            }
            else {
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            }

            release();
        }

        void bind() const {
            glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
        }

        void bind(const unsigned int textureUnit) const {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
        }

        void release() const {
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }

        unsigned int getResolution() const {
            return resolution;
        }

        GLuint getHandle() {
            return handle;
        }
    private:
        const unsigned int DEFAULT_RESOLUTION = 256;

        GLuint handle;
        unsigned int resolution;
    };
}

#endif /* CUBEMAP_H */

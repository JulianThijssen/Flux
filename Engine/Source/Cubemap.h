#pragma once
#ifndef CUBEMAP_H
#define CUBEMAP_H

#include "String.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Flux {
    class Cubemap {
    public:
        Cubemap() : resolution(DEFAULT_RESOLUTION) { }

        void create(const char* paths[6]) {
            glGenTextures(1, &handle);
            bind();

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            for (int i = 0; i < 6; i++) {
                int width, height, bpp;
                unsigned char* data = stbi_load(paths[i], &width, &height, &bpp, STBI_rgb_alpha);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }

            release();
        }

        void createEmpty(const unsigned int width, const unsigned int height) {
            glGenTextures(1, &handle);
            bind();

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            for (int i = 0; i < 6; i++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
            }

            release();
        }

        void bind() const {
            glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
        }

        void release() const {
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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

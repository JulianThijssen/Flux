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

        Texture* texture = create(width, height, bpp, image);

        return texture;
    }

    Texture* TextureLoader::create(const int width, const int height, const int bpp, const unsigned char* data) {
        GLuint handle;
        glGenTextures(1, &handle);

        glBindTexture(GL_TEXTURE_2D, handle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        return new Texture(handle);
    }
}

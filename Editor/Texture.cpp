#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Renderer/RenderState.h"
#include "TextureUnit.h"
#include "Util/Path.h"
#include "Util/Log.h"

namespace Flux {

    bool loadTextureFromFile(Path path, int& width, int& height, TextureType type, void** data) {
        int bpp;
        switch (type) {
        case COLOR: *data = stbi_load(path.str().c_str(), &width, &height, &bpp, STBI_rgb_alpha); break;
        case GREYSCALE: *data = stbi_load(path.str().c_str(), &width, &height, &bpp, STBI_grey); break;
        case HDR: *data = stbi_loadf(path.str().c_str(), &width, &height, &bpp, STBI_rgb_alpha); break;
        }

        if (!*data) {
            Log::error("Failed to load image at: " + path.str());
            return false;
        }

        Log::debug("Loaded texture: " + path.str() + " " + std::to_string(width) + " " + std::to_string(height) + " " + std::to_string(bpp));
        return true;
    }

    Texture::Texture(GLenum target)
        :
        target(target)
    { }

    void Texture::create()
    {
        glGenTextures(1, &handle);

        created = true;
    }

    void Texture::destroy()
    {
        if (!created) { return; }
        glDeleteTextures(1, &handle);

        created = false;
    }

    Texture2D::Texture2D()
        :
        Texture(GL_TEXTURE_2D)
    { }

    bool Texture2D::loadFromFile(Path path, TextureType type)
    {
        int width, height;
        void* data;
        bool loaded = loadTextureFromFile(path, width, height, type, &data);

        if (!loaded) { return false; }

        return true;
    }

    void Texture2D::setData(uint width, uint height, GLint internalFormat, GLenum format, GLenum type, const void* data)
    {
        if (!created) { return; }

        this->width = width;
        this->height = height;

        glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);
    }

    Texture3D::Texture3D()
        :
        Texture(GL_TEXTURE_3D)
    { }

    bool Texture3D::loadFromFile(Path path, TextureType type)
    {
        int width, height;
        void* data;
        bool loaded = loadTextureFromFile(path, width, height, type, &data);

        if (!loaded) { return false; }

        return true;
    }

    void Texture3D::setData(uint width, uint height, uint depth, 
        GLint internalFormat, GLenum format, GLenum type, const void* data)
    {
        this->width = width;
        this->height = height;
        this->depth = depth;

        glTexImage3D(target, 0, internalFormat, width, height, depth, 0, format, type, data);
    }

    Cubemap::Cubemap()
        :
        Texture(GL_TEXTURE_CUBE_MAP)
    { }

    void Cubemap::setData(uint resolution, GLint internalFormat, GLenum format, GLenum type, const void* data)
    {
        this->resolution = resolution;

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, internalFormat, resolution, resolution, 0, format, type, data);
    }

    bool Cubemap::loadFromFiles(std::vector<Path> paths)
    {
        if (paths.size() != 6) { return false; }

        return true;
    }

    void Cubemap::setFace(uint face)
    {
        this->face = face;
    }
}

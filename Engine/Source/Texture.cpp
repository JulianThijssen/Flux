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

    void Texture::bind(const uint textureUnit) const
    {
        if (!created) { return; }
        if (textureUnit > MAX_TEXTURE_UNITS - 1) { return; }

        RenderState::setActiveTexture(textureUnit);
        RenderState::bindTexture(target, handle);
        lastBoundUnit = textureUnit;
    }

    void Texture::release() const
    {
        RenderState::bindTexture(target, 0);
    }

    void Texture::setSampling(Sampling minFilter, Sampling magFilter, Sampling mipFilter)
    {
        if (!isBound()) { bind(lastBoundUnit); }

        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter == NEAREST ? GL_NEAREST : GL_LINEAR);

        if (mipFilter == NONE) {
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter == NEAREST ? GL_NEAREST : GL_LINEAR);
        }
        else if (mipFilter == NEAREST) {
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter == NEAREST ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_NEAREST);
        }
        else if (mipFilter == LINEAR) {
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter == NEAREST ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
        }
    }

    void Texture::setMaxMipmapLevel(uint level)
    {
        if (!isBound()) { bind(lastBoundUnit); }

        glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, level);
    }

    void Texture::generateMipmaps()
    {
        if (!isBound()) { bind(lastBoundUnit); }
        Log::debug("GENERATING MIPMAPS: " + std::to_string(lastBoundUnit));
        glGenerateMipmap(target);
    }

    bool Texture::isBound() const
    {
        return RenderState::getActiveTexture() == handle;
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

        create();
        bind(TextureUnit::TEXTURE0);

        switch (type) {
        case COLOR: setData(width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, data); break;
        case GREYSCALE: setData(width, height, GL_R8, GL_RED, GL_UNSIGNED_BYTE, data); break;
        case HDR: setData(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT, data); break;
        }

        release();

        return true;
    }

    void Texture2D::setData(uint width, uint height, GLint internalFormat, GLenum format, GLenum type, const void* data)
    {
        if (!created) { return; }

        this->width = width;
        this->height = height;

        glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);
    }

    void Texture2D::setWrapping(Wrapping sWrapping, Wrapping tWrapping)
    {
        if (!isBound()) { bind(lastBoundUnit); }

        glTexParameteri(target, GL_TEXTURE_WRAP_S, sWrapping);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, tWrapping);
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

        create();
        bind(TextureUnit::TEXTURE0);

        switch (type) {
        case COLOR: setData(height, height, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, data); break;
        case GREYSCALE: setData(height, height, height, GL_R8, GL_RED, GL_UNSIGNED_BYTE, data); break;
        case HDR: setData(height, height, height, GL_RGBA16F, GL_RGBA, GL_FLOAT, data); break;
        }

        release();

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

    void Texture3D::setWrapping(Wrapping sWrapping, Wrapping tWrapping, Wrapping rWrapping)
    {
        if (!isBound()) { bind(lastBoundUnit); }

        glTexParameteri(target, GL_TEXTURE_WRAP_S, sWrapping);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, tWrapping);
        glTexParameteri(target, GL_TEXTURE_WRAP_R, rWrapping);
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

        create();
        bind(TextureUnit::TEXTURE0);

        for (int i = 0; i < 6; i++)
        {
            int width, height;
            void* data;
            bool loaded = loadTextureFromFile(paths[i], width, height, COLOR, &data);

            if (!loaded) { destroy(); return false; }

            setFace(face);
            setData(width, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        release();

        return true;
    }

    void Cubemap::setFace(uint face)
    {
        this->face = face;
    }

    void Cubemap::setWrapping(Wrapping sWrapping, Wrapping tWrapping, Wrapping rWrapping)
    {
        if (!isBound()) { bind(lastBoundUnit); }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrapping);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrapping);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, rWrapping);
    }
}

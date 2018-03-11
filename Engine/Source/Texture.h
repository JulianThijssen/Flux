#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

typedef unsigned int uint;

namespace Flux {
    class Path;

    enum TextureType {
        COLOR, GREYSCALE, HDR
    };

    enum Sampling {
        NONE,
        NEAREST,
        LINEAR
    };

    enum Wrapping {
        CLAMP = GL_CLAMP_TO_EDGE,
        REPEAT = GL_REPEAT,
        BORDER = GL_CLAMP_TO_BORDER
    };

    class Texture {
    public:
        Texture(GLenum target);

        /**
        * Initializes the use of OpenGL functions in this context and
        * generates the raw OpenGL ID of the texture. This function
        * should be called before doing any other texture operations
        * and only when an OpenGL context is current and active.
        */
        void create();

        /**
        * Deletes the OpenGL texture ID and invalidates this texture
        * until create() is called again.
        */
        void destroy();

        /**
        * Binds the texture to the active texture unit.
        */
        void bind(const uint textureUnit) const;

        /**
        * Unbinds the texture by binding the default (0) texture.
        */
        void release() const;

        void setSampling(Sampling minFilter, Sampling magFilter, Sampling mipFilter = NONE);

        void setMaxMipmapLevel(uint level);

        void generateMipmaps();

        /**
        * Return the raw OpenGL texture ID.
        */
        GLuint getHandle()
        {
            return handle;
        }

        bool isCreated() const
        {
            return created;
        }

    protected:
        // OpenGL 3.3 specifies that at least 16 texture units must be supported per stage.
        static const unsigned int MAX_TEXTURE_UNITS = 16;

        bool created = false;

        GLuint handle;
        GLenum target;
    };

    class Texture2D : public Texture {
    public:
        Texture2D();

        bool loadFromFile(Path path, TextureType type);

        void setData(uint width, uint height, GLint internalFormat, GLenum format, GLenum type, const void* data);
        void setWrapping(Wrapping sWrapping, Wrapping tWrapping);

        uint getWidth() const {
            return width;
        }

        uint getHeight() const {
            return height;
        }
    private:
        uint width, height;
    };

    class Texture3D : public Texture {
    public:
        Texture3D();

        bool loadFromFile(Path path, TextureType type);

        void setData(uint width, uint height, uint depth, 
            GLint internalFormat, GLenum format, GLenum type, const void* data);
        void setWrapping(Wrapping sWrapping, Wrapping tWrapping, Wrapping rWrapping);

        uint getWidth() const {
            return width;
        }

        uint getHeight() const {
            return height;
        }

        uint getDepth() const {
            return depth;
        }
    private:
        uint width, height, depth;
    };
}

#endif /* TEXTURE_H */

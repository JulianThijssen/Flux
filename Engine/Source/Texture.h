#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

typedef unsigned int uint;

namespace Flux {
    class Texture {
    public:
        Texture(GLenum target)
        :
            target(target)
        { }

        /**
        * Initializes the use of OpenGL functions in this context and
        * generates the raw OpenGL ID of the texture. This function
        * should be called before doing any other texture operations
        * and only when an OpenGL context is current and active.
        */
        void create() {
            glGenTextures(1, &handle);

            created = true;
        }

        /**
        * Deletes the OpenGL texture ID and invalidates this texture
        * until create() is called again.
        */
        void destroy()
        {
            if (!created) { return; }
            glDeleteTextures(1, &handle);

            created = false;
        }

        /**
        * Binds the texture to the active texture unit.
        */
        void bind(const uint textureUnit) const {
            if (!created) { return; }
            if (textureUnit > MAX_TEXTURE_UNITS - 1) { return; }

            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(target, handle);
        }

        /**
        * Unbinds the texture by binding the default (0) texture.
        */
        void release() const {
            glBindTexture(target, 0);
        }

        virtual void setData(GLint internalFormat, GLenum format, GLenum type, const void* data) = 0;

        /**
        * Return the raw OpenGL texture ID.
        */
        GLuint getHandle()
        {
            return handle;
        }

    protected:
        // OpenGL 3.3 specifies that at least 16 texture units must be supported per stage.
        static const unsigned int MAX_TEXTURE_UNITS = 16;

        bool created;

        GLuint handle;
        GLenum target;
    };

    class Texture2D : public Texture {
    public:
        Texture2D(uint width, uint height)
        :
            Texture(GL_TEXTURE_2D),
            width(width),
            height(height)
        { }

        void setData(GLint internalFormat, GLenum format, GLenum type, const void* data) override {
            glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);
        }

        uint getWidth() const {
            return width;
        }

        uint getHeight() const {
            return height;
        }
    private:
        uint width, height;
    };
}

#endif /* TEXTURE_H */

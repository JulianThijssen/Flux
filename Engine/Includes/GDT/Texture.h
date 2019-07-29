#pragma once

#include "OpenGL.h"

typedef unsigned int uint;

namespace GDT
{
    enum Sampling
    {
        NONE,
        NEAREST,
        LINEAR
    };

    enum Wrapping
    {
        CLAMP = GL_CLAMP_TO_EDGE,
        REPEAT = GL_REPEAT,
        BORDER = GL_CLAMP_TO_BORDER
    };

    enum TextureUnit;

    class Texture
    {
        friend class Framebuffer;

    public:
        Texture(GLenum target);
        void create();
        void bind(TextureUnit textureUnit) const;
        void release();
        void setSampling(Sampling minFilter, Sampling magFilter, Sampling mipFilter = NONE);
        void destroy();

        bool isCreated() const;

    protected:
        bool created = false;

        GLenum target;
        GLuint handle;
    };

    class Texture2D : public Texture
    {
    public:
        Texture2D();
        void setData(uint width, uint height, GLint internalFormat, GLenum format, GLenum type, const void* data);
        void setWrapping(Wrapping sWrapping, Wrapping tWrapping);

    private:
        uint width, height;
    };
}

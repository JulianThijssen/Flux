#pragma once
#ifndef RENDERER_STATE_H
#define RENDERER_STATE_H

#include <glad/glad.h>

namespace Flux {
    enum Capability {
        BLENDING = GL_BLEND,
        FACE_CULLING = GL_CULL_FACE,
        DEPTH_TEST = GL_DEPTH_TEST,
        STENCIL_TEST = GL_STENCIL_TEST,
        POLYGON_OFFSET = GL_POLYGON_OFFSET_FILL
    };

    class RenderState {
    public:
        static void enable(Capability capability);
        static void disable(Capability capability);
        static void setClearColor(float r, float g, float b, float a);
        
        static void drawQuad();

    private:
        static GLuint quadVao;
    };
}

#endif /* RENDERER_STATE_H */

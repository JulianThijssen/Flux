#pragma once
#ifndef FORWARD_RENDERER_H
#define FORWARD_RENDERER_H

#include "Renderer.h"

#include "Shader.h"
#include "Model.h"

namespace Flux {
    class ForwardRenderer : public Renderer {
    public:
        ForwardRenderer() { }

        virtual void create();
        virtual void update(const Scene& scene);
        virtual void renderScene(const Scene& scene);
        virtual void renderMesh(Entity* e);
    };
}

#endif /* FORWARD_RENDERER_H */

#pragma once
#ifndef FORWARD_RENDERER_H
#define FORWARD_RENDERER_H

#include "Renderer.h"

#include "Shader.h"
#include "Model.h"
#include "Texture.h"

namespace Flux {
    class ForwardRenderer : public Renderer {
    public:
        ForwardRenderer() { }

        virtual void create();
        virtual void update(const Scene& scene);
        virtual void renderScene(const Scene& scene);
        virtual void renderMesh(Entity* e);
    private:
        Texture* tex;
    };
}

#endif /* FORWARD_RENDERER_H */

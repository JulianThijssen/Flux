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

        virtual void update(const Scene& scene);
    };
}

#endif /* FORWARD_RENDERER_H */

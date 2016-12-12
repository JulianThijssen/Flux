#pragma once
#ifndef FORWARD_RENDERER_H
#define FORWARD_RENDERER_H

#include "Renderer.h"

#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include "Cubemap.h"
#include "IrradianceMap.h"

namespace Flux {
    class ForwardRenderer : public Renderer {
    public:
        ForwardRenderer() { }

        virtual bool create();
        virtual void update(const Scene& scene);
        virtual void renderScene(const Scene& scene);
        virtual void renderMesh(Entity* e);
    private:
        Cubemap cubemap;
        IrradianceMap* irradianceMap;
    };
}

#endif /* FORWARD_RENDERER_H */

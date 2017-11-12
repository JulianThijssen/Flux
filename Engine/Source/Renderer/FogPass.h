#pragma once
#ifndef FOG_PASS_H
#define FOG_PASS_H

#include "RenderPhase.h"

#include "Shader.h"

#include <memory>

namespace Flux
{
    class Texture;
    class Texture3D;
    class Framebuffer;

    class FogPass : public RenderPhase
    {
    public:
        FogPass();

        void SetSource(const Texture* source);
        void SetDepthMap(const Texture* depthMap);
        void SetTarget(const Framebuffer* target);

        void render(const Scene& scene) override;

    private:
        std::unique_ptr<Shader> shader;

        const Texture* source;
        const Texture* depthMap;
        const Framebuffer* target;
    };
}

#endif /* FOG_PASS_H */

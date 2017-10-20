#pragma
#ifndef AVERAGE_PASS_H
#define AVERAGE_PASS_H

#include <Engine/Source/RenderPhase.h>

#include <Engine/Source/Shader.h>

#include <memory>

namespace Flux
{
    class Texture;

    class AveragePass : public RenderPhase
    {
    public:
        AveragePass();

        void SetTextures(std::vector<Texture>);
        void render(const Scene& scene) override;

    private:
        static const unsigned int MAX_SOURCES = 8;

        std::unique_ptr<Shader> shader;

        std::vector<Texture> textures;

        std::vector<int> units;
    };
}

#endif /* AVERAGE_PASS_H */

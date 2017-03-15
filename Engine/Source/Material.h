#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include "Texture.h"

namespace Flux {
    class Shader;

    class Material {
    public:
        Texture* diffuseTex;
        Texture* normalTex;
        Texture* metalTex;
        Texture* roughnessTex;

        void bind(Shader& shader) const;
        void release(Shader& shader) const;
    };
}

#endif /* MATERIAL_H */

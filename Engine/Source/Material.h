#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include "Texture.h"

namespace Flux {
    class Material {
    public:
        Texture* diffuseTex;
        Texture* normalTex;
        Texture* metalTex;
        Texture* roughnessTex;
    };
}

#endif /* MATERIAL_H */

#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include "Texture.h"

namespace Flux {
    class Material {
    public:
        Texture* diffuseTex;
        Texture* normalTex;
    };
}

#endif /* MATERIAL_H */

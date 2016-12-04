#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include "Texture.h"

namespace Flux {
    class Material {
    public:
        Texture* diffuseTex;
    };
}

#endif /* MATERIAL_H */

#pragma once
#ifndef MATERIAL_LOADER_H
#define MATERIAL_LOADER_H

#include "Material.h"
#include "Path.h"

namespace Flux {
    class MaterialLoader {
    public:
        static Material* loadMaterial(Path path);
    };
}

#endif /* MATERIAL_LOADER_H */

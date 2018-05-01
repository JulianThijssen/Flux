#pragma once

#include "Material.h"
#include "Util/Path.h"

namespace Flux {
    class MaterialLoader {
    public:
        static Material* loadMaterial(Path path);
    };
}

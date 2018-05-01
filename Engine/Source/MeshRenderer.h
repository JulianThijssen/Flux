#pragma once

#include "Component.h"
#include "Material.h"

namespace Flux {
    class MeshRenderer : public Component {
    public:
        uint32_t materialID;
    };
}

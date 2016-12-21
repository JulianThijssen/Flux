#pragma once
#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include "Component.h"
#include "Material.h"

namespace Flux {
    class MeshRenderer : public Component {
    public:
        uint32_t materialID;
    };
}

#endif /* MESH_RENDERER_H */

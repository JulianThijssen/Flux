#pragma once
#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include "Component.h"
#include "Material.h"

#include <string>

namespace Flux {
    class MeshRenderer : public Component {
    public:
        std::string materialID;
    };
}

#endif /* MESH_RENDERER_H */

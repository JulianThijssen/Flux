#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Material.h"

#include <vector>

namespace Flux {
    class Model {
    public:
        std::vector<Mesh> meshes;
        std::vector<Flux::Material> materials;

        void addMesh(const Mesh& mesh);
        void addMaterial(const Flux::Material& material);
    };
}

#endif /* MODEL_H */

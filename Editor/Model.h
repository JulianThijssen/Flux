#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Material.h"

#include <vector>

namespace Flux {
    namespace Editor {
        class Model {
        public:
            std::vector<Mesh> meshes;
            std::vector<Material> materials;

            void addMesh(const Mesh& mesh);
            void addMaterial(const Material& material);
        };
    }
}

#endif /* MODEL_H */

#pragma once

#include "Mesh.h"
#include "Material.h"

#include <vector>
#include <memory>

namespace Flux {
    namespace Editor {
        class Model {
        public:
            std::vector<std::unique_ptr<Mesh>> meshes;
            std::vector<Material> materials;

            void addMesh(Mesh* const mesh);
            void addMaterial(const Material& material);
        };
    }
}

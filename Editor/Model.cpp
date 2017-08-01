#include "Model.h"

namespace Flux {
    namespace Editor {
        void Model::addMesh(const Mesh& mesh) {
            meshes.push_back(mesh);
        }

        void Model::addMaterial(const Flux::Material& material) {
            materials.push_back(material);
        }
    } // namespace Editor
} // namespace Flux

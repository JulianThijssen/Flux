#include "Model.h"

namespace Flux {
    namespace Editor {
        void Model::addMesh(Mesh* const mesh) {
            meshes.push_back(std::unique_ptr<Mesh>(mesh));
        }

        void Model::addMaterial(const Flux::Material& material) {
            materials.push_back(material);
        }
    } // namespace Editor
} // namespace Flux

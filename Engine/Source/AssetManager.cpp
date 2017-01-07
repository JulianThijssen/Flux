#include "AssetManager.h"

namespace Flux {
    std::unordered_map<uint32_t, Material*> AssetManager::materials;
    std::unordered_map<std::string, Mesh*> AssetManager::meshes;

    uint32_t AssetManager::numMaterials() {
        return (uint32_t) materials.size();
    }

    Material* AssetManager::getMaterial(uint32_t id) {
        return materials[id];
    }

    Mesh* AssetManager::getMesh(std::string id) {
        return meshes[id];
    }

    void AssetManager::addMaterial(uint32_t id, Material* material) {
        materials[id] = material;
    }

    void AssetManager::addMesh(std::string id, Mesh* mesh) {
        meshes[id] = mesh;
    }
}

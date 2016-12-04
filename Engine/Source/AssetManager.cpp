#include "AssetManager.h"

namespace Flux {
    std::unordered_map<std::string, Material*> AssetManager::materials;
    std::unordered_map<std::string, Mesh*> AssetManager::meshes;

    Material* AssetManager::getMaterial(std::string id) {
        return materials[id];
    }

    Mesh* AssetManager::getMesh(std::string id) {
        return meshes[id];
    }

    void AssetManager::addMaterial(std::string id, Material* material) {
        materials[id] = material;
    }

    void AssetManager::addMesh(std::string id, Mesh* mesh) {
        meshes[id] = mesh;
    }
}

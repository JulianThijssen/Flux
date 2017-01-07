#pragma once
#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "Material.h"
#include "Mesh.h"

#include <unordered_map>

namespace Flux {
    class AssetManager {
    public:
        static uint32_t numMaterials();
        static Material* getMaterial(uint32_t id);
        static Mesh* getMesh(std::string id);
        static void addMaterial(uint32_t id, Material* material);
        static void addMesh(std::string id, Mesh* mesh);
    private:
        static std::unordered_map<uint32_t, Material*> materials;
        static std::unordered_map<std::string, Mesh*> meshes;
    };
}

#endif /* ASSET_MANAGER_H */

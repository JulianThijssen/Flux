#pragma once
#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "Material.h"
#include "Mesh.h"

#include <unordered_map>

namespace Flux {
    class AssetManager {
    public:
        static Material* getMaterial(std::string id);
        static Mesh* getMesh(std::string id);
        static void addMaterial(std::string id, Material* material);
        static void addMesh(std::string id, Mesh* mesh);
    private:
        static std::unordered_map<std::string, Material*> materials;
        static std::unordered_map<std::string, Mesh*> meshes;
    };
}

#endif /* ASSET_MANAGER_H */

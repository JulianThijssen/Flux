#include "MaterialLoader.h"

#include "Material.h"
#include "String.h"
#include "File.h"

#include "TextureLoader.h"
#include "Path.h"

#include <vector>
#include <iostream>

namespace Flux {
    Material* MaterialLoader::loadMaterial(Path path) {
        std::vector<String> lines = File::loadLines(path.str().c_str());

        Material* material = new Material();
        for (auto line : lines) {
            vector<String> tokens = line.split(' ');

            String key = tokens[0];
            String value = tokens[1];
            if (key == "Name") {
                Log::debug(value.c_str());
            }
            if (key == "DiffuseTex") {
                Log::debug(value.c_str());
                String path = value;
                material->diffuseTex = TextureLoader::loadTexture(Path(path));
            }
            if (key == "NormalTex") {
                Log::debug(value.c_str());
                String path = value;
                material->normalTex = TextureLoader::loadTexture(Path(path));
            }
            if (key == "MetalTex") {
                Log::debug(value.c_str());
                String path = value;
                material->metalTex = TextureLoader::loadTextureGreyscale(Path(path));
            }
            if (key == "RoughnessTex") {
                Log::debug(value.c_str());
                String path = value;
                material->roughnessTex = TextureLoader::loadTextureGreyscale(Path(path));
            }
            if (key == "StencilTex") {
                Log::debug(value.c_str());
                String path = value;
                material->stencilTex = TextureLoader::loadTextureGreyscale(Path(path));
            }
            if (key == "Tiling") {
                material->tilingX = std::stof(tokens[1].c_str());
                material->tilingY = std::stof(tokens[2].c_str());
            }
        }
        return material;
    }
}

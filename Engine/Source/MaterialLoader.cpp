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
            if (key == "Name") {
                std::cout << tokens[1].c_str() << std::endl;
            }
            if (key == "DiffuseTex") {
                std::cout << tokens[1].c_str() << std::endl;
                String path = tokens[1];
                material->diffuseTex = TextureLoader::loadTexture(Path(path));
            }
            if (key == "NormalTex") {
                std::cout << tokens[1].c_str() << std::endl;
                String path = tokens[1];
                material->normalTex = TextureLoader::loadTexture(Path(path));
            }
            if (key == "MetalTex") {
                std::cout << tokens[1].c_str() << std::endl;
                String path = tokens[1];
                material->metalTex = TextureLoader::loadTexture(Path(path));
            }
            if (key == "RoughnessTex") {
                std::cout << tokens[1].c_str() << std::endl;
                String path = tokens[1];
                material->roughnessTex = TextureLoader::loadTexture(Path(path));
            }
            if (key == "Tiling") {
                material->tilingX = std::stof(tokens[1].c_str());
                material->tilingY = std::stof(tokens[2].c_str());
            }
        }
        return material;
    }
}

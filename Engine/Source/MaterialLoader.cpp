#include "MaterialLoader.h"

#include "Material.h"
#include "Util/String.h"
#include "Util/File.h"

#include "TextureLoader.h"
#include "Util/Path.h"

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
                material->diffuseTex = TextureLoader::loadTexture(Path(path), COLOR, REPEAT, SamplingConfig(LINEAR, LINEAR, LINEAR));
            }
            if (key == "NormalTex") {
                Log::debug(value.c_str());
                String path = value;
                material->normalTex = TextureLoader::loadTexture(Path(path), COLOR, REPEAT, SamplingConfig(LINEAR, LINEAR, LINEAR));
            }
            if (key == "MetalTex") {
                Log::debug(value.c_str());
                String path = value;
                material->metalTex = TextureLoader::loadTexture(Path(path), GREYSCALE, REPEAT, SamplingConfig(LINEAR, LINEAR, LINEAR));
            }
            if (key == "RoughnessTex") {
                Log::debug(value.c_str());
                String path = value;
                material->roughnessTex = TextureLoader::loadTexture(Path(path), GREYSCALE, REPEAT, SamplingConfig(LINEAR, LINEAR, LINEAR));
            }
            if (key == "StencilTex") {
                Log::debug(value.c_str());
                String path = value;
                material->stencilTex = TextureLoader::loadTexture(Path(path), GREYSCALE, REPEAT, SamplingConfig(LINEAR, LINEAR, LINEAR));
            }
            if (key == "EmissionTex") {
                Log::debug(value.c_str());
                String path = value;
                material->emissionTex = TextureLoader::loadTexture(Path(path), COLOR, REPEAT, SamplingConfig(LINEAR, LINEAR, LINEAR));
            }
            if (key == "Emission") {
                material->emission = Vector3f(std::stof(tokens[1].c_str()), std::stof(tokens[2].c_str()), std::stof(tokens[3].c_str()));
            }
            if (key == "Tiling") {
                material->tilingX = std::stof(tokens[1].c_str());
                material->tilingY = std::stof(tokens[2].c_str());
            }
        }
        return material;
    }
}

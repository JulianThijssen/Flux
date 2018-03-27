#include "MaterialLoader.h"

#include "Material.h"
#include "Util/String.h"
#include "Util/File.h"

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
                material->diffuseTex.loadFromFile(Path(path), COLOR);
                material->diffuseTex.setWrapping(REPEAT, REPEAT);
                material->diffuseTex.setSampling(LINEAR, LINEAR, LINEAR);
                material->diffuseTex.generateMipmaps();
            }
            if (key == "NormalTex") {
                Log::debug(value.c_str());
                String path = value;
                material->normalTex.loadFromFile(Path(path), COLOR);
                material->normalTex.setWrapping(REPEAT, REPEAT);
                material->normalTex.setSampling(LINEAR, LINEAR, LINEAR);
                material->normalTex.generateMipmaps();
            }
            if (key == "MetalTex") {
                Log::debug(value.c_str());
                String path = value;
                material->metalTex.loadFromFile(Path(path), GREYSCALE);
                material->metalTex.setWrapping(REPEAT, REPEAT);
                material->metalTex.setSampling(LINEAR, LINEAR, LINEAR);
                material->metalTex.generateMipmaps();
            }
            if (key == "RoughnessTex") {
                Log::debug(value.c_str());
                String path = value;
                material->roughnessTex.loadFromFile(Path(path), GREYSCALE);
                material->roughnessTex.setWrapping(REPEAT, REPEAT);
                material->roughnessTex.setSampling(LINEAR, LINEAR, LINEAR);
                material->roughnessTex.generateMipmaps();
            }
            if (key == "StencilTex") {
                Log::debug(value.c_str());
                String path = value;
                material->stencilTex.loadFromFile(Path(path), GREYSCALE);
                material->stencilTex.setWrapping(REPEAT, REPEAT);
                material->stencilTex.setSampling(LINEAR, LINEAR, LINEAR);
                material->stencilTex.generateMipmaps();
            }
            if (key == "EmissionTex") {
                Log::debug(value.c_str());
                String path = value;
                material->emissionTex.loadFromFile(Path(path), COLOR);
                material->emissionTex.setWrapping(REPEAT, REPEAT);
                material->emissionTex.setSampling(LINEAR, LINEAR, LINEAR);
                material->emissionTex.generateMipmaps();
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

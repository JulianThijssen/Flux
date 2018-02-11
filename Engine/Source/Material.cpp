#include "Material.h"

#include "Shader.h"
#include "TextureUnit.h"

namespace Flux {
    void Material::bind(Shader& shader) const {
        if (diffuseTex) {
            diffuseTex->bind(TextureUnit::ALBEDO);
            shader.uniform1i("material.diffuseMap", TextureUnit::ALBEDO);
            shader.uniform1i("material.hasDiffuseMap", 1);
        }
        if (normalTex) {
            normalTex->bind(TextureUnit::NORMAL);
            shader.uniform1i("material.normalMap", TextureUnit::NORMAL);
            shader.uniform1i("material.hasNormalMap", 1);
        }
        if (metalTex) {
            metalTex->bind(TextureUnit::METALNESS);
            shader.uniform1i("material.metalMap", TextureUnit::METALNESS);
            shader.uniform1i("material.hasMetalMap", 1);
        }
        if (roughnessTex) {
            roughnessTex->bind(TextureUnit::ROUGHNESS);
            shader.uniform1i("material.roughnessMap", TextureUnit::ROUGHNESS);
            shader.uniform1i("material.hasRoughnessMap", 1);
        }
        if (stencilTex) {
            stencilTex->bind(TextureUnit::STENCIL);
            shader.uniform1i("material.stencilMap", TextureUnit::STENCIL);
            shader.uniform1i("material.hasStencilMap", 1);
        }
        if (emissionTex) {
            emissionTex->bind(TextureUnit::EMISSION);
            shader.uniform1i("material.emissionMap", TextureUnit::EMISSION);
            shader.uniform1i("material.hasEmissionMap", 1);
        }
        shader.uniform3f("material.emission", emission);
        shader.uniform2f("material.tiling", tilingX, tilingY);
    }

    void Material::release(Shader& shader) const {
        shader.uniform1i("material.hasDiffuseMap", 0);
        shader.uniform1i("material.hasNormalMap", 0);
        shader.uniform1i("material.hasMetalMap", 0);
        shader.uniform1i("material.hasRoughnessMap", 0);
        shader.uniform1i("material.hasStencilMap", 0);
        shader.uniform1i("material.hasEmissionMap", 0);
    }
}

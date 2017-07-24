#version 150 core

struct Material {
    vec3 diffuseColor;
    
    sampler2D diffuseMap;
    sampler2D normalMap;
    sampler2D metalMap;
    sampler2D roughnessMap;
    sampler2D stencilMap;

    vec2 tiling;

    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasMetalMap;
    bool hasRoughnessMap;
    bool hasStencilMap;
};

uniform Material material;

in vec2 pass_texCoords;

/* Samples a tiled texture */
vec4 sampleTiled(sampler2D tex, vec2 texCoords) {
    return texture(tex, texCoords * material.tiling);
}

void main()
{
    if (material.hasStencilMap) {
        float Stencil = sampleTiled(material.stencilMap, pass_texCoords).r;
        if (Stencil < 0.999) {
            discard;
        }
    }
}

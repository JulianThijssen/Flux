#version 150 core

struct Material {
    vec3 diffuseColor;
    
    sampler2D diffuseMap;
    sampler2D normalMap;
    sampler2D metalMap;
    sampler2D roughnessMap;
    sampler2D stencilMap;

    vec3 emission;
    vec2 tiling;

    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasMetalMap;
    bool hasRoughnessMap;
    bool hasStencilMap;
};

uniform mat4 modelMatrix;

uniform Material material;

uniform vec3 camPos;

in vec3 pass_position;
in vec2 pass_texCoords;
in vec3 pass_normal;
in vec3 pass_tangent;
in vec3 pass_worldPos;

out vec4 fragColor;
out vec4 fragNormal;
out vec4 fragPosition;

/* Samples a tiled texture */
vec4 sampleTiled(sampler2D tex, vec2 texCoords) {
    return texture(tex, texCoords * material.tiling);
}

/* Calculates the normal of the fragment using a normal map */
vec3 calcNormal(vec3 normal, vec3 tangent, vec2 texCoord) {
    vec3 bitangent = cross(normal, tangent);
    vec3 mapNormal = sampleTiled(material.normalMap, texCoord).rgb * 2 - 1;
    
    mat3 TBN = mat3(tangent, bitangent, normal);
    return normalize(TBN * mapNormal);
}

void main() {
    if (material.hasStencilMap) {
        float Stencil = sampleTiled(material.stencilMap, pass_texCoords).r;
        if (Stencil < 0.5) {
            discard;
        }
    }
    
    vec3 P = pass_worldPos;
    vec3 N = pass_normal;

    if (material.hasNormalMap) {
        N = calcNormal(N, pass_tangent, pass_texCoords);
    }
    N = normalize((modelMatrix * vec4(N, 0))).xyz;
    
    vec3 V = normalize(camPos - P);
    vec3 R = normalize(reflect(-V, N));

    float Metalness = 0;
    if (material.hasMetalMap) {
        Metalness = sampleTiled(material.metalMap, pass_texCoords).r;
    }
    
    float Roughness = 1;
    if (material.hasRoughnessMap) {
        Roughness = sampleTiled(material.roughnessMap, pass_texCoords).r;
    }
    
    // Base Color
    vec3 BaseColor = vec3(1);
    if (material.hasDiffuseMap) {
        BaseColor = sampleTiled(material.diffuseMap, pass_texCoords).rgb;
    }
    if (length(material.emission) > 0.001) {
        BaseColor = normalize(material.emission);
    }
    
    fragColor = vec4(BaseColor, Roughness);
    fragNormal = vec4(N * 0.5 + 0.5, Metalness);
    fragPosition = vec4(P, length(material.emission));
}

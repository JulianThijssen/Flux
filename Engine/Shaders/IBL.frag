#version 330 core

struct Material {
    vec3 diffuseColor;
    
    sampler2D diffuseMap;
    sampler2D normalMap;
    sampler2D metalMap;
    sampler2D roughnessMap;

    vec2 tiling;

    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasMetalMap;
    bool hasRoughnessMap;
};

uniform mat4 modelMatrix;

uniform Material material;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterEnvmap;
uniform sampler2D scaleBiasMap;

uniform vec3 camPos;

in vec3 pass_position;
in vec2 pass_texCoords;
in vec3 pass_normal;
in vec3 pass_tangent;

out vec4 fragColor;

#define PI 3.14159265

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

vec3 toLinear(vec3 gammaColor) {
    return pow(gammaColor, vec3(2.2));
}

vec3 ApproximateSpecularIBL(vec3 SpecularColor, float Roughness, vec3 N, vec3 V)
{
    float NdotV = clamp(dot(N, V), 0, 1);
    vec3 R = reflect(-V, N);
    
    vec3 PrefilteredColor = textureLod(prefilterEnvmap, R, Roughness * 5).rgb;
    vec2 EnvBRDF = texture(scaleBiasMap, vec2(Roughness, NdotV)).rg;
    
    return PrefilteredColor * (SpecularColor * EnvBRDF.x + EnvBRDF.y);
}

void main() {
    vec3 position = (modelMatrix * (vec4(pass_position, 1))).xyz;
    vec3 N = pass_normal;
    vec3 V = normalize(camPos - position);

    if (material.hasNormalMap) {
        N = calcNormal(N, pass_tangent, pass_texCoords);
    }
    N = normalize((modelMatrix * vec4(N, 0))).xyz;
    
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
        BaseColor = toLinear(sampleTiled(material.diffuseMap, pass_texCoords).rgb);
    }
    
    vec3 DiffuseColor = BaseColor * (1 - Metalness);
    vec3 SpecularColor = mix(vec3(0.04), BaseColor, Metalness);
    
    vec3 Irradiance = texture(irradianceMap, R).rgb;
    vec3 indirectDiffuse = DiffuseColor * Irradiance;
    vec3 indirectSpecular = ApproximateSpecularIBL(SpecularColor, Roughness, N, V);

    fragColor = vec4(indirectDiffuse + indirectSpecular, 1);
}

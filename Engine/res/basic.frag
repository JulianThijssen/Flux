#version 150 core

struct PointLight {
    vec3 position;
};

struct Material {
    vec3 diffuseColor;
    
    sampler2D diffuseMap;
    sampler2D normalMap;
    sampler2D metalMap;

    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasMetalMap;
};

uniform mat4 modelMatrix;

uniform PointLight pointLight;
uniform Material material;

uniform samplerCube cubemap;
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

/* Calculates the diffuse contribution of the light */
float CosTheta(vec3 N, vec3 L) {
	return clamp(dot(N, normalize(L)), 0, 1);
}

/* Calculates the normal of the fragment using a normal map */
vec3 calcNormal(vec3 normal, vec3 tangent, vec2 texCoord) {
    vec3 bitangent = cross(tangent, normal);
    vec3 mapNormal = texture(material.normalMap, texCoord).rgb * 2 - 1;
    
    mat3 TBN = mat3(tangent, bitangent, normal);
    return normalize(TBN * mapNormal);
}

vec3 Fresnel(vec3 BaseColor, float Metalness, float b) {
    float n1 = 1;
    float n2 = 2.9304;
    //float F0 = pow((n1 - n2) / (n1 + n2), 2);
    vec3 F0 = mix(vec3(0.04), BaseColor, Metalness);
    return F0 + (1 - F0) * pow(1 - b, 5);
}

float GGX(float NdotH, float a) {
    float a2 = a * a;
    float d = NdotH * NdotH * (a2 - 1) + 1;
    return a2 / (PI * d * d);
}

float Schlick(float NdotL, float NdotV, float a) {
    float a1 = a + 1;
    float k = a1 * a1 * 0.125;
    float G1 = NdotL / (NdotL * (1 - k) + k);
    float G2 = NdotV / (NdotV * (1 - k) + k);
    return G1 * G2;
}

vec3 CookTorrance(vec3 n, vec3 v, vec3 h, vec3 l, vec3 BaseColor, float Metalness, float roughness) {
    float NdotH = dot(n, h);
    float NdotV = dot(n, v);
    float NdotL = dot(n, l);
    float D = GGX(NdotH, roughness);
    vec3 F = Fresnel(BaseColor, Metalness, NdotV);
    float G = Schlick(NdotL, NdotV, roughness);
    return (D * F * G) / (4 * NdotL * NdotV);
}

void main() {
    vec3 position = (modelMatrix * (vec4(pass_position, 1))).xyz;
    vec3 N = pass_normal;
    vec3 V = normalize(camPos - position);
    vec3 L = normalize(pointLight.position - position);
    vec3 H = normalize(L + V);

    if (material.hasNormalMap) {
        N = calcNormal(N, pass_tangent, pass_texCoords);
    }
    N = normalize((modelMatrix * vec4(N, 0))).xyz;
    
    vec3 R = normalize(reflect(-V, N));
    
    float CosTheta = CosTheta(N, normalize(L));
    float Metalness = 0;
    
    if (material.hasMetalMap) {
        Metalness = texture(material.metalMap, pass_texCoords).r;
    }
    
    // Base Color
    vec3 BaseColor = vec3(1, 1, 1);
    if (material.hasDiffuseMap) {
        BaseColor = texture(material.diffuseMap, pass_texCoords).rgb;//* (1 - Metalness);
    }

    // Lambert Diffuse BRDF
    vec3 LambertBRDF = (BaseColor / PI);
    
    // Cook Torrance Specular BRDF
    vec3 CookBRDF = CookTorrance(N, V, H, L, BaseColor, Metalness, 0.01);
    
    // Reflection
    //vec3 R = normalize(N * (2 * dot(N, V)) - V);
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 reflection = texture(cubemap, R).rgb;
    vec3 iblSpecular = textureLod(prefilterEnvmap, R, 0).rgb;

    vec3 Li = vec3(10, 10, 10);
    vec3 Radiance = (LambertBRDF + CookBRDF) * Li * CosTheta + (Metalness * reflection);
    vec3 GI = (LambertBRDF * (1 - Metalness) + CookBRDF * Metalness) * irradiance * 10 * CosTheta;

    fragColor = vec4(iblSpecular, 1.0);
}

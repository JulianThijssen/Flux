#version 150 core

#define D_GGX
#define G_Schlick

struct DirectionalLight {
    vec3 direction;
    vec3 color;
};

struct PointLight {
    vec3 position;
    vec3 color;
};

struct Material {
    vec3 diffuseColor;
    
    sampler2D diffuseMap;
    sampler2D normalMap;
    sampler2D metalMap;
    sampler2D roughnessMap;

    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasMetalMap;
    bool hasRoughnessMap;
};

uniform mat4 modelMatrix;

uniform DirectionalLight dirLight;
uniform PointLight pointLight;

uniform bool isPointLight;
uniform bool isDirLight;

uniform Material material;

uniform vec3 camPos;

in vec3 pass_position;
in vec2 pass_texCoords;
in vec3 pass_normal;
in vec3 pass_tangent;

out vec4 fragColor;

#define PI 3.14159265359

/* Calculates the diffuse contribution of the light */
float CosTheta(vec3 N, vec3 L) {
	return clamp(dot(N, L), 0, 1);
}

/* Calculates the normal of the fragment using a normal map */
vec3 calcNormal(vec3 normal, vec3 tangent, vec2 texCoord) {
    vec3 bitangent = cross(normal, tangent);
    vec3 mapNormal = texture(material.normalMap, texCoord).rgb * 2 - 1;
    
    mat3 TBN = mat3(tangent, bitangent, normal);
    return normalize(TBN * mapNormal);
}

/* ------------------------ Fresnel functions -------------------------- */
// Fresnel
vec3 Fresnel(vec3 BaseColor, float Metalness, float b) {
    vec3 F0 = mix(vec3(0.04), BaseColor, Metalness);
    return F0 + (1.0 - F0) * pow(1.0 - b, 5.0);
}

/* ---------------------- Distribution functions ----------------------- */
// GGX
float GGX(float NdotH, float a) {
    float a2 = a * a;
    float d = NdotH * NdotH * (a2 - 1.0) + 1.0;
    return a2 / (PI * d * d);
}

// Beckmann
float Beckmann(float NdotH, float a) {
    float a2 = a * a;
    float r1 = 1.0 / (4.0 * a2 * pow(NdotH, 4.0));
    float r2 = (NdotH * NdotH - 1.0) / (a2 * NdotH * NdotH);
    return r1 * exp(r2);
}

/* --------------------- Geometric shadowing terms --------------------- */
// Schlick
float Schlick(float NdotL, float NdotV, float a) {
    float a1 = a + 1.0;
    float k = a1 * a1 * 0.125;
    float G1 = NdotL / (NdotL * (1.0 - k) + k);
    float G2 = NdotV / (NdotV * (1.0 - k) + k);
    return G1 * G2;
}

// Cook-Torrance
float GCT(float NdotL, float NdotV, float NdotH, float VdotH) {
    float G1 = (2.0 * NdotH * NdotV) / VdotH;
    float G2 = (2.0 * NdotH * NdotL) / VdotH;
    return min(1.0, min(G1, G2));
}

// Keleman
float Keleman(float NdotL, float NdotV, float VdotH) {
    return (NdotL * NdotV) / (VdotH * VdotH);
}

/* --------------------------- Shading model --------------------------- */
// Cook-Torrance
vec3 CookTorrance(vec3 N, vec3 V, vec3 H, vec3 L, vec3 BaseColor, float Metalness, float Roughness) {
    float NdotH = max(0.0, dot(N, H));
    float NdotV = max(1e-7, dot(N, V));
    float NdotL = max(1e-7, dot(N, L));
    float VdotH = max(0.0, dot(V, H));
    #ifdef D_GGX
    float D = GGX(NdotH, Roughness);
    #endif
    #ifdef D_Beckmann
    float D = Beckmann(NdotH, Roughness);
    #endif
    
    #ifdef G_Schlick
    float G = Schlick(NdotL, NdotV, Roughness);
    #endif
    #ifdef G_CookTorrance
    float G = GCT(NdotL, NdotV, NdotH, VdotH);
    #endif
    #ifdef G_Keleman
    float G = Keleman(NdotL, NdotV, VdotH);
    #endif
    
    vec3 F = Fresnel(BaseColor, Metalness, VdotH);
    
    return (D * F * G) / (4.0 * NdotL * NdotV);
}

vec3 toLinear(vec3 gammaColor) {
    return pow(gammaColor, vec3(2.2));
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
    
    vec3 L;
    vec3 Li = vec3(1, 1, 1);
    float Attenuation = 1;
    if (isPointLight) {
        vec3 dir = pointLight.position - position;
        float distance = dot(dir, dir);
        Attenuation = CosTheta(N, normalize(L)) * 1 / distance;
        Li = pointLight.color;
    }
    if (isDirLight) {
        L = -dirLight.direction;
        Attenuation = CosTheta(N, L);
        Li = dirLight.color;
    }
    vec3 H = normalize(L + V);
    
    float Metalness = 0;
    if (material.hasMetalMap) {
        Metalness = texture(material.metalMap, pass_texCoords).r;
    }
    
    float Roughness = 1;
    if (material.hasRoughnessMap) {
        Roughness = texture(material.roughnessMap, pass_texCoords).r;
    }
    
    // Base Color
    vec3 BaseColor = vec3(1);
    if (material.hasDiffuseMap) {
        BaseColor = toLinear(texture(material.diffuseMap, pass_texCoords).rgb);
    }

    vec3 DiffuseColor = BaseColor;
    
    // Lambert Diffuse BRDF
    vec3 LambertBRDF = (DiffuseColor / PI);
    
    // Cook Torrance Specular BRDF
    vec3 CookBRDF = CookTorrance(N, V, H, L, BaseColor, Metalness, Roughness);

    vec3 Radiance = (LambertBRDF + CookBRDF) * Li * Attenuation;

    fragColor = vec4(Radiance, 1.0);
}

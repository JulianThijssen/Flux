#version 150 core

#define PI 3.14159265359

#define D_GGX
#define G_Schlick

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    sampler2DShadow shadowMap;
    mat4 shadowMatrix;
};

struct PointLight {
    vec3 position;
    vec3 color;
};

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D positionMap;

uniform mat4 modelMatrix;

uniform DirectionalLight dirLight;
uniform PointLight pointLight;

uniform bool isPointLight;
uniform bool isDirLight;

uniform vec3 camPos;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

vec3 toLinear(vec3 gammaColor) {
    return pow(gammaColor, vec3(2.2));
}

/* Calculates the diffuse contribution of the light */
float CosTheta(vec3 N, vec3 L) {
	return clamp(dot(N, L), 0, 1);
}

/* ------------------------ Fresnel functions -------------------------- */
// Fresnel
vec3 Fresnel(vec3 BaseColor, float Metalness, float b) {
    vec3 F0 = mix(vec3(0.04), BaseColor, Metalness);
    return F0 + (1.0 - F0) * pow(1.0 - b, 5.0);
}

/* ---------------------- Distribution functions ----------------------- */
// GGX
float GGX(float NdotH, float Roughness) {
    float a = Roughness * Roughness;
    float a2 = a * a;
    float d = NdotH * NdotH * (a2 - 1.0) + 1.0;
    return a2 / (PI * d * d);
}

// Beckmann
float Beckmann(float NdotH, float Roughness) {
    float a = Roughness * Roughness;
    float a2 = a * a;
    float r1 = 1.0 / (4.0 * a2 * pow(NdotH, 4.0));
    float r2 = (NdotH * NdotH - 1.0) / (a2 * NdotH * NdotH);
    return r1 * exp(r2);
}

/* --------------------- Geometric shadowing terms --------------------- */
// Schlick
float Schlick(float NdotL, float NdotV, float Roughness) {
    float a = Roughness + 1.0;
    float k = a * a * 0.125;
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

void main() {
    vec3 BaseColor = toLinear(texture(albedoMap, pass_texCoords).rgb);
    float Roughness = texture(albedoMap, pass_texCoords).w;
    vec3 N = texture(normalMap, pass_texCoords).rgb * 2 - 1;
    float Metalness = texture(normalMap, pass_texCoords).w;
    vec3 P = texture(positionMap, pass_texCoords).rgb;
    
    vec3 V = normalize(camPos - P);
    vec3 R = normalize(reflect(-V, N));
    
    vec4 S = dirLight.shadowMatrix * vec4(P, 1);
    
    vec3 L;
    vec3 Li = vec3(1, 1, 1);
    float Attenuation = 1;
    if (isPointLight) {
        L = pointLight.position - P;
        float distance = dot(L, L);
        L = normalize(L);
        Attenuation = CosTheta(N, L) * 1 / distance;
        Li = pointLight.color;
    }
    if (isDirLight) {
        L = -dirLight.direction;
        Attenuation = CosTheta(N, L);
        Li = dirLight.color;
    }

    vec3 H = normalize(L + V);
    
    float visibility = textureProj(dirLight.shadowMap, S);
    
    // Lambert Diffuse BRDF
    vec3 LambertBRDF = (BaseColor / PI) * (1 - Metalness);
    
    // Cook Torrance Specular BRDF
    vec3 CookBRDF = clamp(CookTorrance(N, V, H, L, BaseColor, Metalness, Roughness), 0, 1);

    vec3 Radiance = (LambertBRDF + CookBRDF) * Li * Attenuation;

    fragColor = vec4(Radiance * visibility, 1.0);
}

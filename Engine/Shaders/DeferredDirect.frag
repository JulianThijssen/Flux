#version 150 core

#define PI 3.14159265359
#define EPSILON 0.0001

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
    samplerCubeShadow shadowMap;
};

struct AreaLight {
    vec3 color;
    vec3 vertices[4];
    sampler2D ampTex;
    sampler2D matTex;
};

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D positionMap;

uniform mat4 modelMatrix;

uniform DirectionalLight dirLight;
uniform PointLight pointLight;
uniform AreaLight areaLight;

uniform bool isPointLight;
uniform bool isDirLight;
uniform bool isAreaLight;

uniform vec3 camPos;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

vec3 toLinear(vec3 gammaColor) {
    return pow(gammaColor, vec3(2.2));
}

/* Calculates the diffuse contribution of the light */
float CosTheta(vec3 N, vec3 L) {
    return max(0, dot(N, L));
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

float vecToDepthVal(vec3 v) {
	vec3 absVec = abs(v);
	float locZcomp = max(absVec.x, max(absVec.y, absVec.z));
	
	float f = 400;
	float n = 0.1;
	float normZcomp = (f + n) / (f - n) - (2*f*n) / (f-n) / locZcomp;
	return (normZcomp + 1.0) * 0.5;

vec3 Evaluate_LTC(vec3 N, vec3 V, vec3 P, mat3 invMat, vec3 vertices[4]) {
    // Construct orthonormal basis around N
    vec3 T1, T2;
    T1 = normalize(V - N*dot(V, N));
    T2 = cross(N, T1);

    // Calculate the inverse matrix for putting vertices in light space
    invMat = invMat * transpose(mat3(T1, T2, N));

    // Apply matrix to all vertices
    vec3 verts[4];
    for (int i = 0; i < 4; i++) {
        verts[i] = normalize(invMat * (vertices[i] - P));
    }

    // Integrate
    float E = 0;
    for (int i = 0; i < 4; i++) {
        vec3 pi = verts[i];
        vec3 pj = verts[(i + 1) % 4];

        float ft = acos(clamp(dot(pi, pj), -0.999, 0.999));
        E += ft * normalize(cross(pi, pj)).z;
    }

    // Make it one-sided
    E = max(0, -E);

    return vec3(E);
}

void main() {
    vec4 arMap = texture(albedoMap, pass_texCoords);
    vec4 nmMap = texture(normalMap, pass_texCoords);
    vec4 peMap = texture(positionMap, pass_texCoords);
    
    vec3 BaseColor = toLinear(arMap.rgb);
    float Roughness = arMap.w;
    vec3 N = normalize(nmMap.rgb * 2 - 1);
    float Metalness = nmMap.w;
    vec3 P = peMap.rgb;
    float Emission = peMap.w;
    
    vec3 V = normalize(camPos - P);
    vec3 R = normalize(reflect(-V, N));
    
    vec4 S = dirLight.shadowMatrix * vec4(P, 1);
    
    vec3 L;
    vec3 Li = vec3(1, 1, 1);
    float visibility = 1;
    float Attenuation = 1;
    
    vec3 Radiance = vec3(0, 0, 0);

    // Lambert Diffuse BRDF
    vec3 LambertBRDF = (BaseColor / PI) * (1 - Metalness);

    if (isPointLight) {
        L = pointLight.position - P;
        float distance = dot(L, L);
        visibility = texture(pointLight.shadowMap, vec4(-L, vecToDepthVal(L)));
        L = normalize(L);
        Attenuation = CosTheta(N, L) * 1 / distance;
        Li = pointLight.color;

        vec3 H = normalize(L + V);

        // Cook Torrance Specular BRDF
        vec3 CookBRDF = clamp(CookTorrance(N, V, H, L, BaseColor, Metalness, Roughness), 0, 1);

        Radiance += (LambertBRDF + CookBRDF) * Li * Attenuation;
    }
    if (isDirLight) {
        L = -dirLight.direction;
        Attenuation = CosTheta(N, L);
        Li = dirLight.color;
        visibility = textureProj(dirLight.shadowMap, S);

        vec3 H = normalize(L + V);

        // Cook Torrance Specular BRDF
        vec3 CookBRDF = clamp(CookTorrance(N, V, H, L, BaseColor, Metalness, Roughness), 0, 1);

        Radiance += (LambertBRDF + CookBRDF) * Li * Attenuation;
    }
    if (isAreaLight) {
        vec3 Li = areaLight.color;
        
        float theta = acos(dot(N, V)) / (PI * 0.5);
        vec2 coords = vec2(Roughness, theta);
        vec4 param = texture(areaLight.matTex, coords);
        mat3 M = mat3(vec3(param.x, 0, param.w), vec3(0, param.z, 0), vec3(param.y, 0, 1));
        mat3 invMat = inverse(M);
        
        vec3 T1, T2;
        T1 = normalize(V - N*dot(V, N));
        T2 = cross(N, T1);
        invMat = invMat * transpose(mat3(T1, T2, N));
        
        vec3 verts[4];
        for (int i = 0; i < 4; i++) {
            verts[i] = normalize(invMat * (areaLight.vertices[i] - P));
        }
        
        float E = 0;
        for (int i = 0; i < 4; i++) {
            vec3 pi = verts[i];
            vec3 pj = verts[(i+1) % 4];
            
            float ft = acos(clamp(dot(pi, pj), -0.999, 0.999));
            E += ft * normalize(cross(pi, pj)).z;
        }
        E *= 1.0 / (2.0 * PI);

        E = max(0, -E);
        
        vec3 Rad = vec3(E) * Li;

        fragColor = vec4(LambertBRDF * Rad, 1);
        return;
    }

    vec3 H = normalize(L + V);
    
    // Cook Torrance Specular BRDF
    vec3 CookBRDF = clamp(CookTorrance(N, V, H, L, BaseColor, Metalness, Roughness), 0, 1);

    vec3 Radiance = (LambertBRDF + CookBRDF) * Li * Attenuation;

    fragColor = vec4(Radiance * visibility, 1.0);
}

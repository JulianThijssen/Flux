#version 150 core

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

#define PI 3.14159265

/* Calculates the diffuse contribution of the light */
float CosTheta(vec3 N, vec3 L) {
	return clamp(dot(N, L), 0, 1);
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

vec3 CookTorrance(vec3 N, vec3 V, vec3 H, vec3 L, vec3 BaseColor, float Metalness, float Roughness) {
    float NdotH = dot(N, H);
    float NdotV = dot(N, V);
    float NdotL = dot(N, L);
    float D = GGX(NdotH, Roughness);
    vec3 F = Fresnel(BaseColor, Metalness, NdotV);
    float G = Schlick(NdotL, NdotV, Roughness);
    return (D * F * G) / (4 * NdotL * NdotV);
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
        Li *= pointLight.color;
    }
    if (isDirLight) {
        L = -dirLight.direction;
        Attenuation = CosTheta(N, L);
        Li *= dirLight.color;
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
    vec3 BaseColor = vec3(1, 1, 1);
    if (material.hasDiffuseMap) {
        BaseColor = toLinear(texture(material.diffuseMap, pass_texCoords).rgb);
    }

    vec3 DiffuseColor = BaseColor * (1 - Metalness);
    
    // Lambert Diffuse BRDF
    //vec3 LambertBRDF = (BaseColor / PI);
    
    // Cook Torrance Specular BRDF
    vec3 CookBRDF = CookTorrance(N, V, H, L, BaseColor, Metalness, Roughness);

    vec3 Radiance = (DiffuseColor + CookBRDF) * Li * Attenuation;

    fragColor = vec4(Radiance, 1.0);
}

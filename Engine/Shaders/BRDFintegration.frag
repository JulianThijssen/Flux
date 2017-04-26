#version 150 core

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

#define PI 3.14159265

float RadicalInverse(uint bits) {
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint NumSamples) {
    return vec2(float(i) / float(NumSamples), RadicalInverse(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, float Roughness, vec3 N) {
    float a = Roughness * Roughness;
    
    float Phi = 2 * PI * Xi.x;
    float CosTheta = clamp(sqrt((1 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y)), 0.0, 1.0);
    float SinTheta = sqrt(1 - CosTheta * CosTheta);
    
    vec3 H = vec3(SinTheta * cos(Phi), SinTheta * sin(Phi), CosTheta);

    vec3 UpVector = abs(N.z) < 0.999 ? vec3(0, 0, 1) : vec3(1, 0, 0);
    vec3 TangentX = normalize(cross(N, UpVector));
    vec3 TangentY = cross(N, TangentX);
    
    // Tangent to world space
    return TangentX * H.x + TangentY * H.y + N * H.z;
}

float Schlick(float NdotL, float NdotV, float Roughness) {
    float a = Roughness * Roughness;
    float k = a / 2.0;
    float G1 = NdotL / (NdotL * (1.0 - k) + k);
    float G2 = NdotV / (NdotV * (1.0 - k) + k);
    return G1 * G2;
}

vec2 IntegrateBRDF(float Roughness, float NdotV)
{
    vec3 V = vec3(sqrt(1.0 - NdotV * NdotV), 0, NdotV);
    
    float A = 0;
    float B = 0;

    uint NumSamples = 1024u;
    for (uint i = 0u; i < NumSamples; i++)
    {
        vec2 Xi = Hammersley(i, NumSamples);
        vec3 H = ImportanceSampleGGX(Xi, Roughness, vec3(0, 0, 1)); // Why (0, 0, 1)?
        vec3 L = reflect(-V, H);
        
        float NdotL = clamp(L.z, 0.0, 1.0);
        float NdotH = clamp(H.z, 0.0, 1.0);
        float VdotH = clamp(dot(V, H), 0.0, 1.0);
        
        if (NdotL > 0)
        {
            float G = Schlick(NdotL, NdotV, Roughness);
            
            float G_Vis = G * VdotH / (NdotH * NdotV);
            float Fc = pow(1 - VdotH, 5);
            A += (1 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }

    return vec2(A, B) / NumSamples;
}

void main() {
    vec2 scaleBias = IntegrateBRDF(pass_texCoords.x, pass_texCoords.y);
    fragColor = vec4(scaleBias.x, scaleBias.y, 0, 1);
}

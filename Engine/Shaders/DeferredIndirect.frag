#version 150 core

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D positionMap;
uniform sampler2D depthMap;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterEnvmap;
uniform sampler2D scaleBiasMap;

uniform vec3 camPos;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

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
    vec3 BaseColor = toLinear(texture(albedoMap, pass_texCoords).rgb);
    float Roughness = texture(albedoMap, pass_texCoords).w;
    vec3 N = texture(normalMap, pass_texCoords).rgb * 2 - 1;
    float Metalness = texture(normalMap, pass_texCoords).w;
    vec3 P = texture(positionMap, pass_texCoords).rgb;
    
    vec3 V = normalize(camPos - P);
    vec3 R = normalize(reflect(-V, N));
    
    vec3 DiffuseColor = BaseColor * (1 - Metalness);
    vec3 SpecularColor = mix(vec3(0.04), BaseColor, Metalness);
    
    vec3 Irradiance = texture(irradianceMap, R).rgb;
    vec3 indirectDiffuse = DiffuseColor * Irradiance;
    vec3 indirectSpecular = ApproximateSpecularIBL(SpecularColor, Roughness, N, V);

    fragColor = vec4(indirectDiffuse + indirectSpecular, 1);
}

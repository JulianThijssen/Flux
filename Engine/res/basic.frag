#version 150 core

struct PointLight {
    vec3 position;
};

struct Material {
    vec3 diffuseColor;
    
    sampler2D diffuseMap;
    sampler2D normalMap;

    bool hasDiffuseMap;
    bool hasNormalMap;
};

uniform mat4 modelMatrix;

uniform PointLight pointLight;
uniform Material material;

uniform samplerCube cubemap;
uniform samplerCube irradianceMap;
uniform vec3 camPos;

in vec3 pass_position;
in vec2 pass_texCoords;
in vec3 pass_normal;
in vec3 pass_tangent;

out vec4 fragColor;

/* Calculates the diffuse contribution of the light */
float calcAlbedo(vec3 normal, vec3 lightDir) {
	return clamp(dot(normal, normalize(lightDir)), 0, 1);
}

/* Calculates the normal of the fragment using a normal map */
vec3 calcNormal(vec3 normal, vec3 tangent, vec2 texCoord) {
    vec3 bitangent = cross(tangent, normal);
    vec3 mapNormal = texture(material.normalMap, texCoord).rgb * 2 - 1;
    
    mat3 TBN = mat3(tangent, bitangent, normal);
    return normalize(TBN * mapNormal);
}

void main() {
    vec3 position = (modelMatrix * (vec4(pass_position, 1))).xyz;
    vec3 normal = pass_normal;
    
    vec3 lightDir = pointLight.position - position;
    
    vec3 color;
    if (material.hasDiffuseMap) {
        color = texture(material.diffuseMap, pass_texCoords).rgb;
    }
    if (material.hasNormalMap) {
        normal = calcNormal(normal, pass_tangent, pass_texCoords);
    }
    normal = normalize((modelMatrix * vec4(normal, 0))).xyz;
    
    float albedo = calcAlbedo(normal, normalize(lightDir)) * 5;
    color *= albedo;
    
    vec3 viewDir = normalize(camPos - position);
    vec3 reflDir = normalize(normal * (2 * dot(normal, viewDir)) - viewDir);
    vec3 irradiance = texture(irradianceMap, pass_normal).rgb;
    vec3 reflection = texture(cubemap, reflDir).rgb;
    
    color *= irradiance;
    //color += reflection * 0.3;

    fragColor = vec4(color, 1.0);
}

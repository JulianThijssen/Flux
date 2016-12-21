#version 150 core

struct PointLight {
    vec3 position;
};

struct Material {
    vec3 diffuseColor;
    
    sampler2D diffuseMap;

    bool hasDiffuseMap;
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

out vec4 fragColor;

/* Calculates the diffuse contribution of the light */
float calcAlbedo(vec3 normal, vec3 lightDir) {
	return clamp(dot(normal, normalize(lightDir)), 0, 1);
}

void main() {
    vec3 position = (modelMatrix * (vec4(pass_position, 1))).xyz;
    vec3 normal = normalize((modelMatrix * vec4(pass_normal, 0))).xyz;
    
    vec3 lightDir = pointLight.position - position;
    
    float albedo = calcAlbedo(normal, normalize(lightDir));

    vec3 color = vec3(albedo*2);
    if (material.hasDiffuseMap) {
        color *= texture(material.diffuseMap, pass_texCoords).rgb;
    }
    
    vec3 viewDir = normalize(camPos - position);
    vec3 reflDir = normalize(normal * (2 * dot(normal, viewDir)) - viewDir);
    vec3 irradiance = texture(irradianceMap, pass_normal).rgb;
    vec3 reflection = texture(cubemap, reflDir).rgb;
    
    color *= irradiance;
    //color += reflection * 0.3;

    fragColor = vec4(color, 1.0);
}

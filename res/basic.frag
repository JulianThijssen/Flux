#version 150 core

struct PointLight {
    vec3 position;
};

uniform PointLight pointLight;

uniform sampler2D diffuseTex;
uniform bool hasAlbedo;

in vec3 pass_position;
in vec2 pass_texCoords;
in vec3 pass_normal;

out vec4 fragColor;

float calcDiffuse(vec3 normal, vec3 lightDir) {
    return dot(normal, lightDir);
}

void main() {
    vec3 lightDir = pointLight.position - pass_position;
    
    float diffuse = calcDiffuse(pass_normal, normalize(lightDir));

    vec3 color = vec3(diffuse);
    if (hasAlbedo) {
        color *= texture(diffuseTex, pass_texCoords).rgb;
    }

    fragColor = vec4(color, 1.0);
}

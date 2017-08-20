#version 150 core

uniform sampler2D texA;
uniform sampler2D texB;
uniform sampler2D texC;
uniform sampler2D texD;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

float weights[4] = float[4](0.25, 0.25, 0.25, 0.25);

void main() {
    vec3 b1 = texture(texA, pass_texCoords).rgb * weights[0];
    vec3 b2 = texture(texB, pass_texCoords).rgb * weights[1];
    vec3 b3 = texture(texC, pass_texCoords).rgb * weights[2];
    vec3 b4 = texture(texD, pass_texCoords).rgb * weights[3];

    fragColor = vec4(b1 + b2 + b3 + b4, 1);
}

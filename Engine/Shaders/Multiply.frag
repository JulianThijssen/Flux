#version 150 core

uniform sampler2D texA;
uniform sampler2D texB;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

void main() {
    fragColor = texture(texA, pass_texCoords) * texture(texB, pass_texCoords);
}

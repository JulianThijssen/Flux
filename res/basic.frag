#version 150 core

in vec2 pass_texCoords;

out vec4 fragColor;

void main() {
    fragColor = vec4(pass_texCoords, 0.0, 1.0);
}

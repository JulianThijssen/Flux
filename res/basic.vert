#version 150 core

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 position;
in vec2 texCoords;
in vec3 normal;

out vec3 pass_position;
out vec2 pass_texCoords;
out vec3 pass_normal;

void main() {
    pass_position = position;
    pass_texCoords = texCoords;
    pass_normal = normal;

    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1);
}

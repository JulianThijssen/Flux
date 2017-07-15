#version 150 core

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 PVM;

in vec3 position;
in vec2 texCoords;
in vec3 normal;
in vec3 tangent;

out vec3 pass_position;
out vec2 pass_texCoords;
out vec3 pass_normal;
out vec3 pass_tangent;

void main() {
    pass_position = position;
    pass_texCoords = texCoords;
    pass_normal = normal;
    pass_tangent = tangent;

    gl_Position = PVM * vec4(position, 1);
}

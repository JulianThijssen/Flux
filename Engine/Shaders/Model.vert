#version 330 core

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 PVM;

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

out vec3 pass_position;
out vec2 pass_texCoords;
out vec3 pass_normal;
out vec3 pass_tangent;
out vec3 pass_worldPos;

void main() {
    pass_position = position.xyz;
    pass_texCoords = texCoords;
    pass_normal = normal;
    pass_tangent = tangent;
    pass_worldPos = (modelMatrix * position).xyz;

    gl_Position = PVM * position;
}

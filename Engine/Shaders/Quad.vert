#version 150 core

int indices[6] = int[](0, 1, 2, 2, 1, 3);

vec3 vertices[4] = vec3[](
    vec3(-1, -1, 0),
    vec3(1, -1, 0),
    vec3(-1, 1, 0),
    vec3(1, 1, 0)
);

vec2 texCoords[4] = vec2[](
    vec2(0, 0),
    vec2(1, 0),
    vec2(0, 1),
    vec2(1, 1)
);

out vec3 pass_position;
out vec2 pass_texCoords;

void main() {
    int index = indices[gl_VertexID];
    
    pass_position = vertices[index];
    pass_texCoords = texCoords[index];
    
    gl_Position = vec4(vertices[index], 1);
}

#version 150 core

//in vec3 position;

int indices[6] = int[](0, 1, 2, 2, 1, 3);

out vec2 pass_texCoords;

void main() {
    int id = gl_VertexID;

    int vid = indices[id];
    float x = (vid % 2) > 0 ? 1 : -1;
    float y = (vid & 2) > 0 ? -1 : 1;
    vec3 position = vec3(x, y, 0);
    
    float tx = (vid % 2) > 0 ? 1 : 0;
    float ty = (vid & 2) > 0 ? 0 : 1;
    vec2 texCoords = vec2(tx, ty);
    pass_texCoords = texCoords;

    gl_Position = vec4(position, 1);
}

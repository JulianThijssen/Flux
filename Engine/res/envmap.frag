#version 150 core

uniform samplerCube envmap;
uniform int face;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

void main() {
    mat3 rots[6] = mat3[](
        mat3(vec3(0, 0, -1), vec3(0, 1, 0), vec3(1, 0, 0)), // Y 90
        mat3(vec3(0, 0, 1), vec3(0, 1, 0), vec3(-1, 0, 0)), // Y 270
        mat3(vec3(1, 0, 0), vec3(0, 0, 1), vec3(0, -1, 0)), // X 90
        mat3(vec3(1, 0, 0), vec3(0, 0, -1), vec3(0, 1, 0)), // X -90
        mat3(vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1)),  // Y 0
        mat3(vec3(-1, 0, 0), vec3(0, 1, 0), vec3(0, 0, -1)) // Y 180
    );

    int width = 256;
    int height = 256;
    
    vec3 normal = normalize(vec3(pass_texCoords * 2 - 1, 1));
    normal = rots[face] * normal;
    vec3 color = vec3(0, 0, 0);
    float div = 0;
    
    for (int i = 0; i < 6; i++) {
        mat3 rot = rots[i];
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                vec3 envCoords = vec3((x-128)/256.0, (y-128)/256.0, 1);
                vec3 dir = normalize(envCoords);
                dir = rot * dir;
                color += texture(envmap, dir).rgb * max(dot(normal, dir), 0);
                div += max(dot(normal, dir), 0);
            }
        }
    }
    
    color /= div;
    
    fragColor = vec4(color, 1);
}

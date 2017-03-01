#version 150 core

uniform samplerCube EnvMap;
uniform int Face;

uniform int textureSize;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

vec3 toLinear(vec3 gammaColor) {
    return pow(gammaColor, vec3(2.2));
}

void main() {
    mat3 rots[6] = mat3[](
        mat3(vec3(0, 0, -1), vec3(0, -1, 0), vec3(-1, 0, 0)), // Right
        mat3(vec3(0, 0, 1), vec3(0, -1, 0), vec3(1, 0, 0)), // Left
        mat3(vec3(1, 0, 0), vec3(0, 0, 1), vec3(0, -1, 0)), // Top
        mat3(vec3(1, 0, 0), vec3(0, 0, -1), vec3(0, 1, 0)), // Bottom
        mat3(vec3(1, 0, 0), vec3(0, -1, 0), vec3(0, 0, -1)), // Front
        mat3(vec3(-1, 0, 0), vec3(0, -1, 0), vec3(0, 0, 1))  // Back
    );

    float halfRes = textureSize / 2.0;
    
    vec3 N = normalize(vec3(pass_texCoords * 2 - 1, -1));
    N = rots[Face] * N;
    vec3 Color = vec3(0, 0, 0);
    float TotalWeight = 0;
    
    for (int i = 0; i < 6; i++) {
        mat3 rot = rots[i];
        for (int x = 0; x < textureSize; x++) {
            for (int y = 0; y < textureSize; y++) {
                vec2 envCoords = vec2(float(x) / textureSize, float(y) / textureSize);
                vec3 dir = normalize(vec3(envCoords * 2 - 1, -1));
                dir = rot * dir;
                Color += texture(EnvMap, dir).rgb * max(dot(N, dir), 0);
                TotalWeight += max(dot(N, dir), 0);
            }
        }
    }
    
    Color /= TotalWeight;
    
    fragColor = vec4(toLinear(Color), 1);
}

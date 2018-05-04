#version 150 core

uniform samplerCube EnvMap;
uniform sampler2D EnvTex;
uniform int Face;
uniform bool Skybox;

uniform int textureSize;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

#define PI 3.1415926535897932384626433832795
#define TWO_PI 6.283185307179586476925286766559

const float ONE_OVER_PI = 1.0 / PI;
const float ONE_OVER_TWO_PI = 1.0 / TWO_PI;

vec3 toLinear(vec3 gammaColor) {
    return pow(gammaColor, vec3(2.2));
}

vec2 toUV(vec3 dir) {
    float phi = atan(dir.z, dir.x);
    float theta = acos(dir.y);
    return vec2(phi * ONE_OVER_TWO_PI, theta * ONE_OVER_PI);
}

vec3 sampleSkybox(vec3 dir) {
    return texture(EnvMap, dir).rgb;
}

vec3 sampleSkysphere(vec3 dir) {
    vec2 uv = toUV(dir);
    return textureLod(EnvTex, uv, 0).rgb;
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
    
    vec3 N = normalize(vec3(pass_texCoords * 2 - 1, -1));
    N = rots[Face] * N;
    vec3 Color = vec3(0, 0, 0);
    float TotalWeight = 0;
    
    for (int i = 0; i < 6; i++) {
        mat3 rot = rots[i];
        for (int x = 0; x < textureSize; x++) {
            for (int y = 0; y < textureSize; y++) {
                vec2 envCoords = vec2(x, y) / textureSize;
                vec3 dir = normalize(vec3(envCoords * 2 - 1, -1));
                dir = rot * dir;

                float weight = max(dot(N, dir), 0);
                if (Skybox) {
                    Color += sampleSkybox(dir) * weight;
                } else {
                    Color += sampleSkysphere(dir) * weight;
                }
                TotalWeight += weight;
            }
        }
    }
    
    Color /= TotalWeight;
    
    if (!Skybox) {
        Color = toLinear(Color);
    }

    fragColor = vec4(Color, 1);
}

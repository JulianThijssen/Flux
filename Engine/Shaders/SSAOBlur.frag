#version 150 core

const int blurSize = 4;

uniform sampler2D tex;
uniform ivec2 windowSize;

in vec2 pass_texCoords;

out vec4 fragColor;

void main() {
    vec2 invSize = 1.0 / windowSize;
    float value = 0;
    vec2 hlim = vec2(float(-blurSize) * 0.5 + 0.5);
    for (int x = 0; x < blurSize; x++) {
        for (int y = 0; y < blurSize; y++) {
            vec2 offset = ((hlim + vec2(float(x), float(y))) * invSize);
            value += textureLod(tex, pass_texCoords + offset, 0).r;
        }
    }
    
    fragColor = vec4(vec3(value / (blurSize * blurSize)), 1);
}

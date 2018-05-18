#version 330 core

const int blurSize = 3;

uniform sampler2D tex;
uniform ivec2 windowSize;

in vec2 pass_texCoords;

out vec4 fragColor;

void main() {
    float value = 0;

    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            value += textureOffset(tex, pass_texCoords, ivec2(x, y)).r;
        }
    }
    
    fragColor = vec4(vec3(value / (blurSize * blurSize)), 1);
}

#version 150 core

uniform sampler2D tex;

in vec2 pass_texCoords;

out vec4 fragColor;

vec3 correctGamma(vec3 gammaColor) {
    return pow(gammaColor, vec3(0.4545));
}

void main() {
    fragColor = vec4(correctGamma(texture(tex, pass_texCoords).rgb), 1);
}

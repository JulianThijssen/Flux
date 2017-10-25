#version 150 core

uniform sampler2D tex;
uniform sampler3D lut;

in vec2 pass_texCoords;

out vec4 fragColor;

void main() {
    vec3 color = texture(tex, pass_texCoords).rbg;
    if (color.r > 1 || color.g > 1 || color.b > 1 || color.r < 0 || color.g < 0 || color.b < 0) {
        fragColor = vec4(1, 0, 1, 1);
        return;
    }
    
    vec3 gradedColor = texture(lut, color).rgb;

    fragColor = vec4(gradedColor, 1);
}

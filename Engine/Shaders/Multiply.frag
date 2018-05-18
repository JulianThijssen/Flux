#version 330 core

uniform sampler2D sources[8];
uniform int sourceCount;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

void main() {
    vec3 color = vec3(1, 1, 1);

    for (int i = 0; i < sourceCount; i++) {
        color *= texture(sources[i], pass_texCoords).rgb;
    }
    
    fragColor = vec4(color, 1);
}

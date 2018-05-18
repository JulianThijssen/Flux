#version 330 core

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

uniform sampler2D tex;
uniform float threshold;

const vec3 toLuma = vec3(0.299, 0.587, 0.114);

void main()
{
    vec3 radiance = textureLod(tex, pass_texCoords, 1).rgb;
    float luma = dot(radiance, toLuma);
    
    if (luma > threshold) {
        fragColor = vec4(radiance, 1);
        return;
    }
    fragColor = vec4(0, 0, 0, 1);
}

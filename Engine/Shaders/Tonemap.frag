#version 330 core

uniform sampler2D source;

uniform int tonemapper;
uniform float exposure;

in vec2 pass_texCoords;

out vec4 fragColor;

vec3 reinhardToneMapping(vec3 color, float exposure)
{
    color *= exposure / ((color / exposure) + 1);
    return color;
}

vec3 filmicToneMapping(vec3 color)
{
    color = max(vec3(0), color - vec3(0.004));
    color = (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7) + 0.06);
    return color;
}

void main() {
    vec3 radiance = texture(source, pass_texCoords).rgb;
    
    switch(tonemapper)
    {
    case 0: fragColor = vec4(reinhardToneMapping(radiance, exposure), 1); return;
    case 1: fragColor = vec4(filmicToneMapping(radiance), 1); return;
    }
}

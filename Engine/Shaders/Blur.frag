#version 150 core

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

uniform sampler2D tex;
uniform vec2 direction;

vec3 blur(int lod, vec2 uv, vec2 resolution, vec2 direction) {
    vec3 color = vec3(0);
    vec2 off = vec2(1.333333333333) * direction;
    color += textureLod(tex, uv, lod).rgb * 0.29411764705882354;
    color += textureLod(tex, uv + (off / resolution), lod).rgb * 0.35294117647058826;
    color += textureLod(tex, uv - (off / resolution), lod).rgb * 0.35294117647058826;
    return color;
}

void main()
{
    vec3 color = vec3(0);
    color += blur(4, pass_texCoords, vec2(120, 67.5), direction) * 0.2;
    color += blur(3, pass_texCoords, vec2(240, 135), direction) * 0.2;
    color += blur(2, pass_texCoords, vec2(480, 270), direction) * 0.2;
    //color += blur(1, pass_texCoords, vec2(960, 540), direction) * 0.333333333333;
    //color += blur(0, pass_texCoords, vec2(1920, 1080), direction) * 0.333333333333;
    fragColor = vec4(color, 1);
}

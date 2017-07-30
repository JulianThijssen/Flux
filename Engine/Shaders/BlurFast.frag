#version 150 core

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

uniform ivec2 windowSize;
uniform sampler2D tex;
uniform vec2 direction;
uniform int mipmap;

vec3 blur(int lod, vec2 uv, ivec2 resolution, vec2 direction) {
    vec3 color = vec3(0);
    vec2 off = vec2(1.333333333333) * direction;
    color += textureLod(tex, uv, lod).rgb * 0.29411764705882354;
    color += textureLod(tex, uv + (off / resolution), lod).rgb * 0.35294117647058826;
    color += textureLod(tex, uv - (off / resolution), lod).rgb * 0.35294117647058826;
    return color;
}

void main()
{
    vec3 color = blur(mipmap, pass_texCoords, windowSize, direction);
    fragColor = vec4(color, 1);
}

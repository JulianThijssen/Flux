#version 330 core

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

uniform ivec2 windowSize;
uniform sampler2D tex;
uniform vec2 direction;
uniform int mipmap;

vec3 blur(int lod, vec2 uv, ivec2 resolution, vec2 direction) {
    vec4 color = vec4(0);
    vec2 off1 = vec2(1.411764705882353) * direction;
    vec2 off2 = vec2(3.2941176470588234) * direction;
    vec2 off3 = vec2(5.176470588235294) * direction;
    color += textureLod(tex, uv, lod) * 0.1964825501511404;
    color += textureLod(tex, uv + (off1 / resolution), lod) * 0.2969069646728344;
    color += textureLod(tex, uv - (off1 / resolution), lod) * 0.2969069646728344;
    color += textureLod(tex, uv + (off2 / resolution), lod) * 0.09447039785044732;
    color += textureLod(tex, uv - (off2 / resolution), lod) * 0.09447039785044732;
    color += textureLod(tex, uv + (off3 / resolution), lod) * 0.010381362401148057;
    color += textureLod(tex, uv - (off3 / resolution), lod) * 0.010381362401148057;
    return color.rgb;
}

void main()
{
    vec3 color = blur(mipmap, pass_texCoords, windowSize, direction);
    fragColor = vec4(color, 1);
}

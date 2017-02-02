#version 150 core

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

uniform sampler2D tex;

void main()
{
    fragColor = texture(tex, pass_texCoords);
}

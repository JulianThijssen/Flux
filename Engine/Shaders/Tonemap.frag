#version 150 core

uniform sampler2D tex;

in vec2 pass_texCoords;

out vec4 fragColor;

vec3 reinhardToneMapping(vec3 color, float exposure)
{
	color *= exposure / ((color / exposure) + 1);
	return color;
}

void main() {
    vec3 radiance = texture(tex, pass_texCoords).rgb;
    
    fragColor = vec4(reinhardToneMapping(radiance, 1.5), 1);
}

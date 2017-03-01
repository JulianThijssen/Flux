#version 150 core

uniform samplerCube skybox;

uniform vec2 persp;
uniform mat4 cameraBasis;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

vec3 toLinear(vec3 gammaColor) {
    return pow(gammaColor, vec3(2.2));
}

void main()
{
    vec2 uv = pass_texCoords * 2 - 1;
    
    vec3 cx = cameraBasis[0].xyz;
    vec3 cy = cameraBasis[1].xyz;
    vec3 cz = cameraBasis[2].xyz;
    vec3 direction = normalize(persp.x * cx * uv.x + persp.y * cy * uv.y + cz);
    
    gl_FragDepth = 1;
    fragColor = vec4(toLinear(texture(skybox, direction).rgb), 1);
}

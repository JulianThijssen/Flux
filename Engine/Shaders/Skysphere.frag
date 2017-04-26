#version 150 core

uniform sampler2D tex;

uniform vec2 persp;
uniform mat4 cameraBasis;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

#define PI 3.1415926535897932384626433832795
#define TWO_PI 6.283185307179586476925286766559

const float ONE_OVER_PI = 1.0 / PI;
const float ONE_OVER_TWO_PI = 1.0 / TWO_PI;

vec3 toLinear(vec3 gammaColor) {
    return pow(gammaColor, vec3(2.2));
}

vec2 toUV(vec2 phiTheta) {
    return vec2(phiTheta.x * ONE_OVER_TWO_PI, phiTheta.y * ONE_OVER_PI);
}

void main()
{
    vec2 uv = pass_texCoords * 2 - 1;
    
    vec3 cx = cameraBasis[0].xyz;
    vec3 cy = cameraBasis[1].xyz;
    vec3 cz = cameraBasis[2].xyz;
    vec3 direction = normalize(persp.x * cx * uv.x + persp.y * cy * uv.y + cz);

    float phi = atan(direction.z, direction.x);
    float theta = acos(direction.y);
    uv = toUV(vec2(phi, theta));
    
    gl_FragDepth = 1;
    vec3 color = textureLod(tex, uv, 0).rgb;
    fragColor = vec4(color, 1);
}

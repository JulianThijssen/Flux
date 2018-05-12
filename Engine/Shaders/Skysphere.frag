/**
* The middle of the environment map should be mapped to (0, 0, -1)
* This means (0, 0, -1) should be mapped to (PI, PI/2), which in turn
* should be mapped to (0.5, 0.5).
* The GLSL atan(y, x) function returns values in the range [-PI, PI].
*
*    x    z        atan(z, x)
*    1    0        0
*    0    1        PI/2
*   -1    0        PI and -PI
*    0   -1       -PI/2
*
*    y             acos(y)
*   -1             PI
*    0             PI/2
*    1             0
*
*    y             asin(y)
*   -1            -PI/2
*    0             0
*    1             PI/2
*
*
* So in order to map x=0, z=-1 to PI we need to do atan(z, x) and subtract PI/2.
* And in order to map y=0 to PI/2 we need to do asin(y) and add PI/2.
* We do asin(-y) instead because the environment map is loaded in upside-down.
*/

#version 150 core

uniform sampler2D tex;

uniform vec2 persp;
uniform mat4 cameraBasis;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

const float PI = 3.1415926535897932384626433832795;
const float PI_OVER_TWO = PI / 2.0;
const float TWO_PI = PI * 2.0;

const float ONE_OVER_PI = 1.0 / PI;
const float ONE_OVER_TWO_PI = 1.0 / TWO_PI;

vec3 toLinear(vec3 gammaColor) {
    return pow(gammaColor, vec3(2.2));
}

vec2 toUV(vec3 dir) {
    float phi = atan(dir.z, dir.x) - PI_OVER_TWO;
    float theta = asin(-dir.y) + PI_OVER_TWO;
    return vec2(phi * ONE_OVER_TWO_PI, theta * ONE_OVER_PI);
}

void main()
{
    vec2 uv = pass_texCoords * 2 - 1;
    
    vec3 cx = cameraBasis[0].xyz;
    vec3 cy = cameraBasis[1].xyz;
    vec3 cz = cameraBasis[2].xyz;
    vec3 direction = normalize(persp.x * cx * uv.x + persp.y * cy * uv.y + cz);
    
    uv = toUV(direction);
    
    gl_FragDepth = 1;
    vec3 color = min(textureLod(tex, uv, 0).rgb, 1000);
    fragColor = vec4(color, 1);
}

#version 330 core

uniform sampler2D tex;

uniform vec2 persp;
uniform mat4 cameraBasis;
uniform vec3 sun;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

#define PI 3.1415926535897932384626433832795
#define TWO_PI 6.283185307179586476925286766559
#define E 2.7182818284590452353602874

const float ONE_OVER_PI = 1.0 / PI;
const float ONE_OVER_TWO_PI = 1.0 / TWO_PI;

const vec3 skyColor = vec3(0.2684, 0.05688, 0.03778);
const vec3 horizonColor = vec3(1.3599, 0.83077, 0.2247);

vec3 toLinear(vec3 gammaColor) {
    return pow(gammaColor, vec3(2.2));
}

vec2 toUV(vec2 phiTheta) {
    return vec2(phiTheta.x * ONE_OVER_TWO_PI, phiTheta.y * ONE_OVER_PI);
}

float sigmoid(float x, float sharpness, float shift) {
    return 1 / (1 + pow(E, -sharpness*x + shift));
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
    uv = toUV(vec2(phi, theta)) * 2 - 1;
    
    // Sun
    float sunPower = pow(max(0, -dot(direction, sun)), 1000) * 10;

    gl_FragDepth = 1;
    vec3 color = mix(skyColor, horizonColor, sigmoid(uv.y, 5, 0)) + vec3(10, 2, 1.0) * sunPower;
    fragColor = vec4(color, 1);
}

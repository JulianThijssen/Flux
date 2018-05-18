#version 330 core

uniform sampler2D tex;
uniform sampler2D depthMap;

uniform float zNear;
uniform float zFar;
uniform vec3 fogColor;

in vec2 pass_texCoords;

out vec4 fragColor;

void main() {
    vec3 radiance = texture(tex, pass_texCoords).rgb;
    float z = texture(depthMap, pass_texCoords).r;
    float n = zNear;
    float f = zFar;
    float depth = (2 * n) / (f + n - z * (f - n));
    
    if (z > 0.9999) {
        fragColor = vec4(radiance, 1);
        return;
    }
    
    vec3 color = mix(radiance, fogColor, clamp(depth*2, 0, 1));
    fragColor = vec4(color, 1);
}

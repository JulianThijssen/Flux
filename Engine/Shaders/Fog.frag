#version 150 core

uniform sampler2D tex;
uniform sampler2D depthMap;

in vec2 pass_texCoords;

out vec4 fragColor;

void main() {
    vec3 radiance = texture(tex, pass_texCoords).rgb;
    float z = texture(depthMap, pass_texCoords).r;
    float f = 400;
    float n = 0.1;
    float depth = (2 * n) / (f + n - z * (f - n));
    
    if (z > 0.9999) {
        fragColor = vec4(radiance, 1);
        return;
    }
    
    vec3 color = mix(radiance, vec3(165.0/255, 96.0/255, 81.0/255), clamp(depth*2, 0, 1));
    fragColor = vec4(color, 1);
}

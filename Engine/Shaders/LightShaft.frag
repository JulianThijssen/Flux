#version 150 core

uniform sampler2D sourceTex;

uniform float exposure;
uniform float density;
uniform float decay;
uniform vec2 lightPos;

in vec2 pass_texCoords;

out vec4 fragColor;

const int NUM_STEPS = 100;
const float MAX_LUMA = 100;

const vec3 toLuma = vec3(0.299, 0.587, 0.114);

void main()
{
    vec2 uv = pass_texCoords;
    vec2 stepVector = vec2(lightPos - uv);

    stepVector *= density / float(NUM_STEPS);
    float illuminationDecay = 1.0;

    vec3 color = vec3(0, 0, 0);

    for (int i = 0; i < NUM_STEPS; i++)
    {
        uv += stepVector;

        vec3 sample = texture(sourceTex, uv).rgb;
        float luma = dot(sample, toLuma);
        float lumaClamp = luma > MAX_LUMA ? MAX_LUMA / luma : 1.0;
        
        color += sample * lumaClamp * illuminationDecay;

        illuminationDecay *= decay;
    }

    fragColor = vec4(color * exposure, 1);
}

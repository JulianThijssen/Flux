#version 150 core

uniform sampler2D normalMap;
uniform sampler2D positionMap;
uniform sampler2D depthMap;

uniform sampler2D noiseMap;
uniform vec3 kernel[32];
uniform int kernelSize;

uniform ivec2 windowSize;
uniform vec3 camPos;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

vec3 ProjectVector(vec3 v, vec3 u)
{
    return u * dot(v, u);
}

mat3 CreateOrthonormalBasis(vec3 u, vec3 v)
{
    vec3 tangent = normalize(v - ProjectVector(v, u));
    vec3 bitangent = cross(u, tangent);
    return mat3(tangent, bitangent, u);
}

void main() {
    float Depth = texture(depthMap, pass_texCoords).r;
    if (Depth == 1)
        discard;

    vec3 P = texture(positionMap, pass_texCoords).rgb;
    vec3 N = texture(normalMap, pass_texCoords).rgb * 2 - 1;

    vec3 V = normalize(camPos - P);
    
    // Transform position and normal to view space
    vec3 viewP = (viewMatrix * vec4(P, 1)).xyz;
    vec3 viewN = (viewMatrix * vec4(N, 0)).xyz;
    
    // Perform Gram-Schmidt process to determine orthonormal basis in normal direction
    vec3 rotation = texture(noiseMap, pass_texCoords * windowSize/4).xyz * 2.0 - 1.0;
    mat3 TBN = CreateOrthonormalBasis(viewN, rotation);
    
    // Apply kernel
    float occlusion = 0.0;
    float radius = 0.01;
    for (int i = 0; i < kernelSize; ++i) {
        vec3 sample = TBN * (kernel[i]);
        sample = sample + viewP;

        // Get the position of the sample in NDC
        vec4 depthKernel = projMatrix * vec4(sample, 1.0);
        // Perspective divide
        depthKernel.xyz /= depthKernel.w;
        // Get position from [-1, 1] to [0, 1] space
        depthKernel.xyz = depthKernel.xyz * 0.5 + 0.5;

        // Sample the depth at the sample position
        float sampleDepth = textureLod(depthMap, depthKernel.xy, 0).r;
        
        // Check if the sample is not so far in front or behind the view position that it's useless
        float rangeCheck = float(abs(Depth - sampleDepth) < radius);
        // If the sample is deeper than the depth at the sample position then it's occluded
        occlusion += float(depthKernel.z > sampleDepth) * rangeCheck;
    }
    float visibility = 1.0 - (occlusion / kernelSize);

    fragColor = vec4(vec3(visibility), 1);
}

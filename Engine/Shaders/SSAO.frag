#version 150 core

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D positionMap;
uniform sampler2D depthMap;

uniform sampler2D noiseMap;
uniform vec3 kernel[13];
uniform int kernelSize;

uniform ivec2 windowSize;
uniform vec3 camPos;
uniform float zNear;
uniform float zFar;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

float linearDepth(float depthSample)
{
    depthSample = 2.0 * depthSample - 1.0;
    float zLinear = 2.0 * zNear * zFar / (zFar + zNear - depthSample * (zFar - zNear));
    return -zLinear;
}

void main() {
    vec3 P = texture(positionMap, pass_texCoords).rgb;
    vec3 N = texture(normalMap, pass_texCoords).rgb * 2 - 1;
    
    vec3 V = normalize(camPos - P);
    
    // SSAO
    vec3 viewN = (viewMatrix * vec4(N, 0)).xyz;
    vec3 viewP = (viewMatrix * vec4(P, 1)).xyz;
    
    // Calculate SSAO basis
    vec3 rvec = texture(noiseMap, pass_texCoords * windowSize/4).xyz * 2.0 - 1.0;
    vec3 tangent = normalize(rvec - viewN * dot(rvec, viewN));
    vec3 bitangent = cross(viewN, tangent);
    mat3 TBN = mat3(tangent, bitangent, viewN);
    
    // Apply kernel
    float occlusion = 0.0;
    float radius = 0.2;
    for (int i = 0; i < kernelSize; ++i) {
        vec3 sample = TBN * kernel[i];
        sample = sample + viewP;
        
        vec4 offset = projMatrix * vec4(sample, 1.0);
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;

        float sampleDepth = linearDepth(textureLod(depthMap, offset.xy, 0).r);
        
        float rangeCheck = float(abs(viewP.z - sampleDepth) < radius);
        // If the sample is further on the -Z axis than the depth then it's occluded
        occlusion += float(sample.z <= sampleDepth) * rangeCheck;
    }
    float visibility = 1.0 - (occlusion / kernelSize);

    fragColor = vec4(vec3(visibility * visibility), 1);
}

#version 150 core

#define PI 3.14159265359

uniform int numVertices;
uniform vec3 vertices[10];
uniform vec3 emission;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D positionMap;

uniform sampler2D ampTex;
uniform sampler2D matTex;

uniform mat4 modelMatrix;

uniform vec3 camPos;

in vec3 pass_position;
in vec2 pass_texCoords;

out vec4 fragColor;

vec3 toLinear(vec3 gammaColor) {
    return pow(gammaColor, vec3(2.2));
}

/* Calculates the diffuse contribution of the light */
float CosTheta(vec3 N, vec3 L) {
	return clamp(dot(N, L), 0, 1);
}

void main() {
    vec3 BaseColor = toLinear(texture(albedoMap, pass_texCoords).rgb);
    float Roughness = texture(albedoMap, pass_texCoords).w;
    vec3 N = texture(normalMap, pass_texCoords).rgb * 2 - 1;
    float Metalness = texture(normalMap, pass_texCoords).w;
    vec3 P = texture(positionMap, pass_texCoords).rgb;
    
    vec3 V = normalize(camPos - P);
    vec3 R = normalize(reflect(-V, N));
    
    vec3 Li = emission;

    float theta = clamp(acos(dot(N, V)) / (PI * 0.5), 0.0, 1.0);
       
    vec2 coords = vec2(Roughness, theta);

    vec4 param = texture(matTex, coords);

    mat3 M = mat3(vec3(param.x, 0, param.w), vec3(0, param.z, 0), vec3(param.y, 0, 1));
    mat3 invMat = inverse(M);
    //mat3 invMat = mat3(vec3(1, 0, b), vec3(0, c, 0), vec3(d, 0, a));
    
    vec3 T1, T2;
    T1 = normalize(V - N*dot(V, N));
    T2 = cross(N, T1);
    invMat = invMat * transpose(mat3(T1, T2, N));
    
    vec3 verts[10];
    for (int i = 0; i < numVertices; i++) {
        verts[i] = normalize(invMat * (vertices[i] - P));
    }

    float E = 0;
    for (int i = 0; i < numVertices; i++) {
        vec3 pi = verts[i];
        vec3 pj = verts[(i+1) % numVertices];
        
        float ft = acos(clamp(dot(pi, pj), -0.999, 0.999));
        E += ft * normalize(cross(pi, pj)).z;
    }
    E *= 1.0 / (2.0 * PI);

    E = Emission > EPSILON ? abs(E) : max(0, -E);
    
    // Lambert Diffuse BRDF
    vec3 LambertBRDF = (BaseColor / PI) * (1 - Metalness);

    vec3 Radiance = vec3(E) * Li;

    fragColor = vec4(vec3(Radiance), 1);
}

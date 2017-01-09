#version 150 core

uniform samplerCube skybox;

in vec3 pass_position;

out vec4 fragColor;

void main()
{
    fragColor = texture(skybox, pass_position);
}

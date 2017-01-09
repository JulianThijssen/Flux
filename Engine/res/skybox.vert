#version 150 core

uniform mat4 projMatrix;

in vec3 position;

out vec3 pass_position;

void main()
{
    vec4 projPos = projMatrix * vec4(position, 1);
    gl_Position = projPos.xyww;
    pass_position = position;
}

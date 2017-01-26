#version 150 core

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 position;

out vec3 pass_position;

void main()
{
    vec4 projPos = projMatrix * viewMatrix * modelMatrix * vec4(position, 1);
    gl_Position = projPos.xyww;
    pass_position = position;
}

#version  330 core

layout(location = 0) in vec4 rectArgs;

void main()
{
    gl_Position = rectArgs;
}
#version 330 core 

out vec4 color;

in float zPos;

void main()
{
    color = vec4(-zPos/1000, 0, 0, 1);
}

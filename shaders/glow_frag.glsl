#version 330 core 

out vec4 color;

in float zPos;

void main()
{
    color = vec4(clamp(zPos, 0, 1), 0, 0, 1);
}

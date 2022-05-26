#version  330 core
layout(location = 0) in vec3 vertPos;

out vec2 fragTex;

void main()
{
    gl_Position = vec4(vertPos, 1);
    fragTex = (vertPos.xy+vec2(1, 1))/2.0;
}

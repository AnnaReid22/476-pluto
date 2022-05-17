#version 330 core

in vec2 fragTex;
out vec4 color;

uniform sampler2D gLightOutput;
uniform sampler2D particleTex;

void main()
{
    color = texture(gLightOutput, fragTex);
}
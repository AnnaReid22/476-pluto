#version 330 core

layout(location = 0) out vec4 gBuffer;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gColor;

in vec3 fragPos;
in vec3 fragNor;
in vec2 fragTex;

uniform sampler2D albedoMap;
//uniform sampler2D normalMap;

void main()
{
    gNormal = vec4(normalize(fragNor), 1.0f);
    gBuffer = vec4(fragPos, 1.0f);
    gColor = vec4(texture(albedoMap, fragTex).rgb, 1.0f);
}
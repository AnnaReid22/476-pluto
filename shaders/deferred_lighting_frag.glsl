#version 330 core

in vec2 fragTex;
out vec4 color;

uniform sampler2D gBuffer;
uniform sampler2D gNormal;
uniform sampler2D gColor;

uniform vec3 lightPos;

void main()
{
    vec3 wPos = texture(gBuffer, fragTex).xyz;
    vec3 ldir = normalize(lightPos - wPos);
    vec3 norm = normalize(texture(gNormal, fragTex).xyz);
    vec3 texCol = texture(gColor, fragTex).rgb;
    
    float diff = max(dot(norm, ldir), 0.0);
    //color.rgb = (diff * texCol);
    color.rgb = texCol;
    color.a = 1.0;
}
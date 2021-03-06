#version 330 core

in vec2 fragTex;
out vec4 color;

uniform sampler2D inTexture;

void main()
{
    vec3 inColor = texture(inTexture, fragTex).rgb;
    float brightness = dot(inColor, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.95)
        color = vec4(inColor, 1.0);
    else
        color = vec4(0.0, 0.0, 0.0, 1.0);
}
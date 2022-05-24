#version 330 core

in vec2 fragTex;
out vec4 color;

uniform sampler2D psColorOutput;
uniform sampler2D gLightOutput;
uniform sampler2D psPositionOutput;
uniform sampler2D gBuffer;
uniform sampler2D shadowOutput;
uniform mat4 V;

void main()
{
    // vec4 psCol = texture(psColorOutput, fragTex);
    // vec4 gLight = texture(gLightOutput, fragTex);
    // vec4 psPos = texture(psPositionOutput, fragTex);
    // vec4 gPos = texture(gBuffer, fragTex);
    vec4 shadow = texture(shadowOutput, fragTex);
    // vec4 viewPos = V*vec4(gPos.xyz, 1.0);

    // color.rgb = psCol.rgb*psCol.a + shadow.rgb*(1.0f-psCol.a);
    // color.a = 1.0f;
    color = shadow;
}
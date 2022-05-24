#version 330 core

in vec2 fragTex;
out vec4 color;

uniform sampler2D psColorOutput;
uniform sampler2D gLightOutput;
uniform sampler2D psPositionOutput;
uniform sampler2D gBuffer;
uniform sampler2D skyColorOutput;
uniform sampler2D lazerGlowOutput;

uniform mat4 V;


vec4 getLazerGlow(float radius){
    vec4 sampled = texture(lazerGlowOutput, fragTex);
    if(sampled.r == 0){
        return vec4(0);
    }

    float depth = sampled.r;

    
    float realRadius = radius / (1+depth);
    float delta = realRadius/5.5;

    float percentGlow = 0;
    for(float i=-realRadius; i<=realRadius; i+=delta){
        for(float j=-realRadius; j<=realRadius; j+=delta){
            if(texture(lazerGlowOutput, fragTex + vec2(i, j)).r != 0){
                percentGlow += 1;
            }
        }
    }
    percentGlow /= 121.0;
    
    return vec4(1, 0, 0, clamp(percentGlow, 0, 1));
}

void main()
{
    vec4 psCol = texture(psColorOutput, fragTex);
    vec4 gLight = texture(gLightOutput, fragTex);
    vec4 psPos = texture(psPositionOutput, fragTex);
    vec4 gPos = texture(gBuffer, fragTex);
    vec4 skyCol = texture(skyColorOutput, fragTex);
    vec4 lazerGlow = getLazerGlow(0.01);
    vec4 viewPos = V*vec4(gPos.xyz, 1.0);

    gPos.a *= 2.5007; // gPos.a values arent ranging from 0-1 for some reason... the max is about (1 / 2.5007).. No idea why.

    color.rgb = (1-gPos.a) * skyCol.rgb;

    color.rgb += gPos.a * (psCol.rgb*psCol.a + gLight.rgb*(1.0f-psCol.a));

    color.rgb += lazerGlow.a * lazerGlow.rgb;
    
}
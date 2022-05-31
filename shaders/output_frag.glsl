#version 330 core

in vec2 fragTex;
out vec4 color;

uniform sampler2D psColorOutput;
uniform sampler2D gLightOutput;
uniform sampler2D psPositionOutput;
uniform sampler2D gBuffer;
uniform sampler2D skyColorOutput;
uniform sampler2D lazerGlowOutput;
uniform sampler2D bloomOutput;
uniform mat4 V;


float getLazerGlow(float radius){
    float percentGlow = 0;
    float nSamples = 0;
    if(texture(lazerGlowOutput, fragTex).r == 0){
        return 0;
    }

    for(float i=0; i<10; i++){
        for(float j=0; j<10; j++){
            vec2 offset = vec2(i - 4.5, j - 4.5) / 4.5;
            nSamples += 1;
            if(texture(lazerGlowOutput, fragTex + radius * offset).r != 0){
                percentGlow += 1;
            }
        }
    }
    return 0.5 * clamp(percentGlow / nSamples, 0.5, 1);
}

void main()
{
    vec4 psCol = texture(psColorOutput, fragTex);
    vec4 gLight = texture(gLightOutput, fragTex);
    vec4 psPos = texture(psPositionOutput, fragTex);
    vec4 gPos = texture(gBuffer, fragTex);
    vec4 bloomColor = texture(bloomOutput, fragTex);
    vec4 skyCol = texture(skyColorOutput, fragTex);
    float lazerDepth = texture(lazerGlowOutput, fragTex).r;
    vec4 viewPos = V*vec4(gPos.xyz, 1.0);

    float lazerGlow = getLazerGlow(0.005 * (1-lazerDepth));
    
    gPos.a *= 2.5007; // gPos.a values arent ranging from 0-1 for some reason... the max is about (1 / 2.5007).. No idea why.

    color.rgb = (1-gPos.a) * skyCol.rgb;

    float gDepth = 1;
    if(gPos.a != 0){
        gDepth = -viewPos.z/1000;
    }
    
    if(lazerDepth < gDepth){
        color.r += lazerGlow;
    }

    color.rgb += psCol.rgb*psCol.a + (gLight.rgb + bloomColor.rgb)*(1.0f-psCol.a);

    //color.rgb = vec3(lazerDepth, gDepth, 0);
}
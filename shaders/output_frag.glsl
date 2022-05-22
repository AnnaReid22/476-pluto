#version 330 core

in vec2 fragTex;
out vec4 color;

uniform sampler2D gLightOutput;
uniform sampler2D gBuffer;
//uniform sampler2D particleTex;
uniform sampler2D skyboxRenderTexture;

void main()
{
    vec4 gPos = texture(gBuffer, fragTex);
    if(gPos.a == 1){
        color = texture(skyboxRenderTexture, fragTex);
    }else{
        color = texture(gLightOutput, fragTex);
    }
    color.rgb = gPos.gba;
    //color = texture(skyboxRenderTexture, fragTex);

}
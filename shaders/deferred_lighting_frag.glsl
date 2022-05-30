#version 330 core

float shadow(vec4 LSfPos);

in vec2 fragTex;
out vec4 color;

uniform sampler2D gBuffer;
uniform sampler2D gNormal;
uniform sampler2D gColor;
uniform vec3 lightPos;
uniform sampler2D depthMap;
uniform mat4 LS;

void main()
{
    float shade;

    vec3 wPos = texture(gBuffer, fragTex).xyz;
    vec3 ldir = normalize(lightPos - wPos);
    vec3 norm = normalize(texture(gNormal, fragTex).xyz);

    vec4 posLS = LS*vec4(wPos, 1.0);
    shade = shadow(posLS);

    float diff = max(dot(norm, ldir), 0.0);
    if(shade == 1.0)
    {
        color = diff * (1.0-(shade * 0.4)) * texture(gColor, fragTex);
    }
        
    else
    {
        color = diff * texture(gColor, fragTex);
    }
}

/* returns 1 if shadowed */
float shadow(vec4 LSfPos) 
{
    vec3 projCoord = 0.5*(LSfPos.xyz + vec3(1.0));
	float curD = projCoord.z - 0.005f;
    float lightDepth = texture(depthMap, projCoord.xy).r; 
    if (curD > lightDepth) {
        return 1.0;
    }
    else {
        return 0.0;
    }
}

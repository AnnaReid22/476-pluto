#version 330 core

layout(location = 0) out vec4 gBuffer;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gColor;

in vec3 fragPos;
in vec3 fragNor;
in vec2 fragTex;
in vec3 fragTan;
in vec3 fragBN;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;

uniform bool useNormalMap;

void main()
{
    vec3 normal = normalize(fragNor);

    if(useNormalMap)
    {
        mat3 TBN = mat3(normalize(fragTan), normalize(fragBN), normalize(fragNor));
        vec3 normalMapValue = texture(normalMap, fragTex).xyz;

        normalMapValue = normalize((normalMapValue * 2.0) - vec3(1));

        normal = TBN * normalMapValue;
    }

    gNormal = vec4(normalize(normal), 1.0f);
    gBuffer = vec4(fragPos, 0.4);
    gColor = vec4(texture(albedoMap, fragTex).rgb, 1.0f);
}
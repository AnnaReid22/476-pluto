#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
layout(location = 3) in vec3 vertTan;
layout(location = 4) in vec3 vertBN;


uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform bool useNormalMap;

out vec3 fragPos;
out vec3 fragNor;
out vec2 fragTex;
out vec3 fragTan;
out vec3 fragBN;

void main()
{
	gl_Position = P * V * M * vec4(vertPos, 1.0);
	fragPos = (M * vec4(vertPos, 1.0)).xyz;

	fragNor = (M * vec4(vertNor, 0.0)).xyz;

	if(useNormalMap)
	{
		fragTan = (M * vec4(vertTan, 0.0)).xyz;
		fragBN = (M * vec4(vertBN, 0.0)).xyz;
	}

	fragTex = vertTex;
}
// #version 330 core

// uniform sampler2D Texture0;

// in vec2 vTexCoord;

// out vec4 Outcolor;

// //interpolated normal and light vector in camera space
// in vec3 fragNor;
// in vec3 lightDir;
// //position of the vertex in camera space
// in vec3 EPos;

// void main() {
//   vec4 texColor0 = texture(Texture0, vTexCoord);

//   vec3 normal = normalize(fragNor);
//   vec3 light = normalize(lightDir);
//   float dC = max(0, dot(normal, light));
//   Outcolor = vec4(dC*texColor0.xyz, 1.0);

//   //to confirm texture coordinates
//   //Outcolor = vec4(vTexCoord.x, vTexCoord.y, 0, 0);
// }

#version 330 core
uniform sampler2D Texture0;

in vec2 vTexCoord;
out vec4 Outcolor;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float MatShine;
uniform int flip;

//interpolated normal and light vector in camera space
in vec3 fragNor;
in vec3 lightDir;
//position of the vertex in camera space
in vec3 ePos;

void main()
{
	vec3 normal;
	vec4 texColor0 = texture(Texture0, vTexCoord);
	if(flip == 0)
		normal = normalize(fragNor);
	else
		normal = normalize(fragNor) * -1;
	vec3 light = normalize(lightDir);
	float dC = max(0, dot(normal, light));
	vec3 halfV = normalize(ePos) + normalize(light);
	float sC = pow(max(dot(normalize(halfV), normal), 0), MatShine);
	Outcolor = vec4(0.5*texColor0 + dC*texColor0 + sC*texColor0);
}
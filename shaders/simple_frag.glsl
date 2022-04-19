#version 330 core 

out vec4 color;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float MatShine;

//interpolated normal and light vector in camera space
in vec3 fragNor;
in vec3 lightDir;
//position of the vertex in camera space
in vec3 EPos;

void main()
{
	//you will need to work with these for lighting
	vec3 normal = normalize(fragNor);
	vec3 light = normalize(lightDir);
	vec3 view = normalize(EPos);
	vec3 halfway = normalize(light + view);

	float spec = pow(max(0.0, dot(normal, halfway)), MatShine);
	float Dc = max(0.0, dot(normal, light));

	color = vec4(((vec3(Dc) * MatDif * vec3(1.0)) + (MatAmb * vec3(1.0)) + (vec3(spec) * MatSpec * vec3(1.0))), 1.0);
}

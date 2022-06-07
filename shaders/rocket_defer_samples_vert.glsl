#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform float deformFactor;

out vec3 fragPos;
out vec3 fragNor;
out vec2 fragTex;

float hash(float n)
{
    return fract(sin(n) * 753.5453123);
}

vec3 snoiseRotation(vec3 x)
{
    vec3 rotation;
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);
    float n = p.x + p.y * 157.0 + 113.0 * p.z;
    rotation.x = mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),mix(hash(n + 15.0), hash(n + 300.0), f.x), f.y),mix(mix(hash(n + 78.0), hash(n + 43.0), f.x),mix(hash(n + 88.0), hash(n + 53.0), f.x), f.y), f.z);
    rotation.y = mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),mix(hash(n + 133.0), hash(n + 67.0), f.x), f.y),mix(mix(hash(n + 215.0), hash(n + 328.0), f.x),mix(hash(n + 112.0), hash(n + 14.0), f.x), f.y), f.z);
    rotation.z = mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),mix(hash(n + 99.0), hash(n + 156.0), f.x), f.y),mix(mix(hash(n + 245.0), hash(n + 114.0), f.x),mix(hash(n + 270.0), hash(n + 275.0), f.x), f.y), f.z);
    rotation.x = sin(rotation.x*111.);
    rotation.y = sin(rotation.y*111.);
    rotation.z = sin(rotation.z*111.);
    return rotation;
}

void main()
{
	
	gl_Position = P * V * M * vec4(vertPos - snoiseRotation(vertNor)*deformFactor, 1.0);

	fragNor = (M * vec4(vertNor, 0.0)).xyz;
	
	fragPos = (M * vec4(vertPos, 1.0)).xyz;
	fragTex = vertTex;
}
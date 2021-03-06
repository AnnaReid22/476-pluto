#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out float zPos;

void main()
{
  
	gl_Position = V * M * vec4(vertPos, 1);
  zPos = gl_Position.z;
  gl_Position = P * gl_Position;
}

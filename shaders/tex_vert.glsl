#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform vec3 lightPos;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;

out vec2 vTexCoord;
out vec3 fragNor;
out vec3 lightDir;
out vec3 ePos;

void main() {
  vec4 vPosition;
  gl_Position = P * V * M * vec4(vertPos, 1);
	fragNor = (V * M * vec4(vertNor, 0.0)).xyz;
	lightDir = (V * vec4(lightPos, 1)).xyz - (V * M * vec4(vertPos, 1)).xyz;
	ePos = -1 * vec3(V * M * vec4(vertPos, 1));
  vTexCoord = vertTex;
}

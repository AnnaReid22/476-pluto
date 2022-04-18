// #version  330 core
// layout(location = 0) in vec3 vertPos;
// layout(location = 1) in vec3 vertNor;
// layout(location = 2) in vec2 vertTex;
// uniform mat4 P;
// uniform mat4 M;
// uniform mat4 V;

// out vec3 fragNor;
// out vec3 lightDir;
// out vec3 EPos;
// out vec2 vTexCoord;

// void main() {
//   vec3 lightPos = vec3(10, 10, 0);

//   /* First model transforms */
//   vec3 wPos = vec3(M * vec4(vertPos.xyz, 1.0));
//   gl_Position = P * V *M * vec4(vertPos.xyz, 1.0);

//   fragNor = (V*M * vec4(vertNor, 0.0)).xyz;

//   lightDir = (V*(vec4(5.0, 5.0, -0.5, 0.0))).xyz;
//   EPos = vec3(1); //PULLED for release
  
//   /* pass through the texture coordinates to be interpolated */
//   vTexCoord = vertTex;
// }
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

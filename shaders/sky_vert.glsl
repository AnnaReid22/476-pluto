#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 SkyMat;
uniform float time;

out vec3 vert_sphereCoord;
out vec2 vert_texCoord;

void main() {
  gl_Position = SkyMat * vec4(vertPos, 1);
  vert_sphereCoord = normalize(vertPos);
  vert_texCoord = vertTex;
}

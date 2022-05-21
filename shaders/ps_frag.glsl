#version 330 core

layout(location = 0) out vec4 psPositionOutput;
layout(location = 1) out vec4 psColorOutput;

uniform sampler2D alphaTexture;
in vec3 partCol;
in vec4 partPos;
out vec4 outColor;


void main()
{
	float alpha = texture(alphaTexture, gl_PointCoord).r;
	psColorOutput = vec4(partCol, alpha);
	psPositionOutput = vec4(partPos.xyz, 1.0);
}

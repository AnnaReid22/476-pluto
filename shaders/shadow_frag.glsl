#version 330 core
uniform sampler2D Texture0;
uniform sampler2D shadowDepth;

out vec4 Outcolor;

in OUT_struct {
   vec3 fPos;
   vec3 fragNor;
   vec2 vTexCoord;
   vec4 fPosLS;
   vec3 vColor;
} in_struct;

/* returns 1 if shadowed */
/* called with the point projected into the light's coordinate space */
float TestShadow(vec4 LSfPos) {
  
	// 1: shift the coordinates from -1, 1 to 0 ,1
  vec3 projCoord = 0.5*(LSfPos.xyz + vec3(1.0)); //shifted to texture space so that we can compare them to the texture coordinates
	float curD = projCoord.z - 0.005f;
  // 2: read off the stored depth (.) from the ShadowDepth, using the shifted.xy
  float lightDepth = texture(shadowDepth, projCoord.xy).r; 
	// 3: compare to the current depth (.z) of the projected depth
  
	// 4: return 1 if the point is shadowed
  if (curD > lightDepth)
  {
    return 1.0; //shadowed is true. current depth is farther away than what the light can see!!!!
  }
  else
  {
	  return 0.0;
  }

}

void main() {

  float Shade;
  float amb = 0.3;

  vec4 BaseColor = vec4(in_struct.vColor, 1);
  vec4 texColor0 = texture(Texture0, in_struct.vTexCoord);

  Shade = TestShadow(in_struct.fPosLS);

  Outcolor = amb*(texColor0) + (1.0-Shade)*texColor0*BaseColor;
}


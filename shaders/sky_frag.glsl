#version 330 core

in vec2 vert_texCoord;
in vec3 vert_sphereCoord;
out vec4 outColor;

uniform sampler2D TwinkleNoise;
uniform sampler2D Texture0;
uniform float time;

vec3 get_twinkle_noise(vec3 sphereCoord)
{
		vec3 percents = 1-abs(asin(sphereCoord) * 0.63662);

		vec3 twinkleX = texture(TwinkleNoise, vec2(atan(sphereCoord.y, sphereCoord.z), sphereCoord.x)).rgb;
		vec3 twinkleY = texture(TwinkleNoise, vec2(atan(sphereCoord.x, sphereCoord.z), sphereCoord.y)).rgb;
		vec3 twinkleZ = texture(TwinkleNoise, vec2(atan(sphereCoord.x, sphereCoord.y), sphereCoord.z)).rgb;
		vec3 col = twinkleX * percents.x + twinkleY * percents.y + twinkleZ * percents.z;
		return vec3(1) - col;
}

void main()
{
		vec3 sphereCoord = normalize(vert_sphereCoord);
		vec3 twinkle = get_twinkle_noise(sphereCoord);
		vec3 twinkle2 = get_twinkle_noise(-sphereCoord);


		float minTwinkle = twinkle.x < twinkle.y ? (twinkle.x < twinkle.z ? twinkle.x : twinkle.z) : (twinkle.y < twinkle.z ? twinkle.y : twinkle.z);
		
		float t = twinkle2.x;
		float m = cos(time + t * 6.28);
		m = m * 0.5 + 0.5;
		
		vec3 col = vec3(minTwinkle, minTwinkle, minTwinkle) * m;


		outColor = vec4(col, 1);
		//random rgb
		//time
		//col
		//
		//outColor = vec4(1);
}

#version 330 core

in vec2 vert_texCoord;
in vec3 vert_sphereCoord;
out vec4 outColor;

uniform sampler2D TwinkleNoise;
uniform float time;


vec3 get_twinkle_noise(vec3 sphereCoord)
{
		vec3 percents = 1-abs(asin(sphereCoord) * 0.63662);
		
		vec3 twinkleX = texture(TwinkleNoise, vec2(atan(sphereCoord.y, sphereCoord.z), sphereCoord.x)).rgb;
		vec3 twinkleY = texture(TwinkleNoise, vec2(atan(sphereCoord.z, sphereCoord.x), sphereCoord.y)).rgb;
		vec3 twinkleZ = texture(TwinkleNoise, vec2(atan(sphereCoord.x, sphereCoord.y), sphereCoord.z)).rgb;
		vec3 col = twinkleX * percents.x + twinkleY * percents.y + twinkleZ * percents.z;
		return col;
}

void main()
{
		vec3 sphereCoord = normalize(vert_sphereCoord);
		
		vec3 twinkle = get_twinkle_noise(sphereCoord);

		float period = time;

		vec2 percents = vec2(0.5) + 0.5*vec2(sin(period), sin(time + 3.14159));

		float brightness = twinkle.r*percents.x + twinkle.g + twinkle.b*percents.y;

		outColor = vec4(brightness, brightness, brightness, 1);
}

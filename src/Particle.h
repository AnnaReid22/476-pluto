#include <vector>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class Particle
{

public:

	Particle(vec3 start, vec3 vel, float life, vec4 col);
	void load(vec3 start, vec4 color, vec3 vel_max, vec3 vel_min, float life);
	void rebirth(float time, vec3 start, vec3 vel_max, vec3 vel_min, vec4 col, float life);
	void update(float time, float h, const glm::vec3 &g, const vec3 start);
	vec3 getPosition() { return position; };
	vec3 getVelocity() { return velocity; };
	vec4 getColor() { return color; };
	float getLifespan() { return lifespan; };
	void setLifespan(float dt);
	void setPosition(vec3 pos);
	void setColor(vec4 color);
	float randFloat(float l, float h);
	
private:

	vec4 color;
	vec3 position;
	vec3 velocity;
	float lifespan;
};
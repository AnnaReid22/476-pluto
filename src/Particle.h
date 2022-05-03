#include <vector>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class Particle
{

public:

	Particle(vec3 pos);
	void load(vec3 start);
	void rebirth(float time, vec3 start);
	void update(float time, float h, const glm::vec3 &g, const vec3 start);
	vec3 getPosition() { return position; };
	vec3 getVelocity() { return velocity; };
	vec4 getColor() { return color; };
	float getLifespan() { return lifespan; };
	void setLifespan(float dt);
	void setPosition(vec3 pos);
	
private:

	vec4 color;
	vec3 position;
	vec3 velocity;
	float lifespan;
};
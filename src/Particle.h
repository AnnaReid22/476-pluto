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
	const vec3 &getPosition() const { return position; };
	const vec3 &getVelocity() const { return velocity; };
	const vec4 &getColor() const { return color; };
	const float &getLifespan() const { return lifespan; };
	void setLifespan(float dt);
	
private:

	vec4 color;
	vec3 position;
	vec3 velocity;
	float lifespan;
};
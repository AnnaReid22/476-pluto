#include <iostream>
#include "Particle.h"

Particle::Particle(vec3 start) :
	position(start),
	velocity(0.0f, 0.0f, 0.0f),
	lifespan(1.0f),
	color(1.0f, 1.0f, 1.0f, 1.0f)
{
}

void Particle::load(vec3 start)
{
	rebirth(0.0f, start);
}

float randFloat(float l, float h)
{
	float r = rand() / (float) RAND_MAX;
	return (1.0f - r) * l + r * h;
}

void Particle::rebirth(float time, vec3 start)
{
	position = start;
	velocity.x = randFloat(-0.1f, 0.1f);
	velocity.y = randFloat(0.2f, 0.5f);
	velocity.z = randFloat(-0.3f, 0.3f);
	lifespan = randFloat(2.0f, 5.0f); 
   	color.r = randFloat(0.6f, 1.0f);
   	color.g = randFloat(0.6f, 1.0f);
   	color.b = randFloat(0.6f, 1.0f);
	color.a = 1.0f;
}

void Particle::setLifespan(float dt)
{
	lifespan = lifespan - dt;
}

#include <iostream>
#include "Particle.h"

Particle::Particle(vec3 start, vec3 vel, float life, vec4 col) :
	position(start),
	velocity(vel),
	lifespan(life),
	color(col)
{
}

void Particle::load(vec3 start, vec4 color, vec3 vel_max, vec3 vel_min, float life)
{
	rebirth(0.0f, start, vel_max, vel_min, color, life);
}

float randFloat(float l, float h)
{
	float r = rand() / (float) RAND_MAX;
	return (1.0f - r) * l + r * h;
}

void Particle::rebirth(float time, vec3 start, vec3 vel_max, vec3 vel_min, vec4 col, float life)
{
	position = start;
	velocity.x = randFloat(vel_max.x, -1*vel_min.x);
	velocity.y = randFloat(vel_max.y, -1*vel_min.y);
	velocity.z = randFloat(vel_max.z, -1*vel_min.z);
	lifespan = life; 
   	color = col;
}

void Particle::setLifespan(float dt)
{
	lifespan = lifespan - dt;
}

void Particle::setPosition(vec3 pos)
{
	position += pos;
}

void Particle::setColor(vec4 col)
{
	color += col;
}

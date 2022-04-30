#include <random>
#include "ParticleSystem.h"
#include "GameObject.h"
#include <iostream>

//reference: https://learnopengl.com/In-Practice/2D-Game/Particles

unsigned int lastDeadParticle = 0;

void ParticleSystem::Start()
{
  time = time->getInstance();

  //initialize particles
  for (unsigned int i = 0; i < numParticles; ++i)
    particles.push_back(Particle(vec3(0.0)));
}

void ParticleSystem::Update()
{
  unsigned int new_particles = 2;

  // add new particles
  for (unsigned int i = 0; i < new_particles; ++i)
  {
      int dead = deadParticle();
      rebirth(particles[dead], object, offset);
  }

  // update other particles
  for (unsigned int i = 0; i < numParticles; ++i)
  {
      Particle &p = particles[i];
      p.setLifespan(dt);
      if (p.getLifespan() > 0.0f)
      {	// particle is alive, thus update
          p.getPosition() -= p.getVelocity() * dt;
          p.getColor().a -= dt * 2.5f;
      }
  }
}

unsigned int deadParticle()
{
    // search from last used particle, this will usually return almost instantly
    for (unsigned int i = lastUsedParticle; i < newParticles; ++i) {
        if (particles[i].Life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (unsigned int i = 0; i < lastUsedParticle; ++i) {
        if (particles[i].Life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // override first particle if all others are alive
    lastUsedParticle = 0;
    return 0;
}  

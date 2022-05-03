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
      particles[dead].load(vec3(0.0));
  }

  // update other particles
  for (unsigned int i = 0; i < numParticles; ++i)
  {
      Particle &p = particles[i];
      p.setLifespan(time->getFrametime());
      if (p.getLifespan() > 0.0f)
      {
          p.getPosition() -= p.getVelocity() * (float)time->getFrametime();
          //p.getColor() -= time->getFrametime() * 2.5f;
      }
  }
}

unsigned int ParticleSystem::deadParticle()
{
    for (unsigned int i = lastDeadParticle; i < numParticles; ++i) {
        if (particles[i].getLifespan() <= 0.0f){
            lastDeadParticle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (unsigned int i = 0; i < lastDeadParticle; ++i) {
        if (particles[i].getLifespan() <= 0.0f){
            lastDeadParticle = i;
            return i;
        }
    }
    // override first particle if all others are alive
    lastDeadParticle = 0;
    return 0;
}  


void ParticleSystem::GPUSetup() {

  // cout << "end: " << start.x << " " << start.y << " " <<start.z << endl;
	//generate the VAO
   glGenVertexArrays(1, &vertArrObj);
   glBindVertexArray(vertArrObj);

   //generate vertex buffer to hand off to OGL - using instancing
   glGenBuffers(1, &vertBuffObj);
   //set the current state to focus on our vertex buffer
   glBindBuffer(GL_ARRAY_BUFFER, vertBuffObj);

   //actually memcopy the data - only do this once
   glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points[0], GL_STREAM_DRAW);

    glGenBuffers(1, &colorbuff);
   glBindBuffer(GL_ARRAY_BUFFER, colorbuff);
   glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), &pointColors[0], GL_STREAM_DRAW);
   assert(glGetError() == GL_NO_ERROR);
	
}
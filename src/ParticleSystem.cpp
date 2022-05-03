#include <random>
#include "ParticleSystem.h"
#include "GameObject.h"
#include "GLSL.h"
#include <iostream>

//reference: https://learnopengl.com/In-Practice/2D-Game/Particles

unsigned int lastDeadParticle = 0;

void ParticleSystem::Start()
{
  time = time->getInstance();
}

void ParticleSystem::Update()
{
  unsigned int new_particles = 2;

  // add new particles
  for (unsigned int i = 0; i < new_particles; ++i)
  {
      int dead = deadParticle();
      particles[dead].load(start);
  }

  // update other particles
  for (unsigned int i = 0; i < numParticles; ++i)
  {
      Particle &p = particles[i];
      p.setLifespan(time->getFrametime());
      if (p.getLifespan() > 0.0f)
      {
          p.getPosition() -= p.getVelocity() * (float)time->getFrametime();
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

void ParticleSystem::draw(std::shared_ptr<Program> prog) {
    glBindVertexArray(vertArrObj);

    int c_pos = prog->getAttribute("colorPos");
    GLSL::enableVertexAttribArray(c_pos);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuff);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    assert(glGetError() == GL_NO_ERROR);
    int h_pos = prog->getAttribute("vertPos");
    GLSL::enableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, vertBuffObj);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);

    glDrawArraysInstanced(GL_POINTS, 0, 1, numParticles);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glDisableVertexAttribArray(0);
}

void ParticleSystem::GPUSetup() {

	for (int i=0; i < numParticles; i++) 
    {
        //defining locations for the points in the array buffer for vertices
        points[i*3+0] = start.x;
        points[i*3+1] = start.y;
        points[i*3+2] = start.z;
        //defining colors for the points in the array buffer for colors
        //matches up to vertex buffer
        pointColors[i*4+0] = 0.5; 
        pointColors[i*4+1] = 0.2;
        pointColors[i*4+2] = 0.7;
        pointColors[i*4+3] = 0.1;
        
        Particle particle = Particle(start);
        particles.push_back(particle);
        particle.load(start);
	}
    std::cout << "particles size: " << particles.size() << std:: endl;

    //generate the VAO
    // vertex array for keeping track of the vertices of the particle system
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

    //code binds vertex buffers and color buffers to GPU
	
}
#include <random>
#include "ParticleSystem.h"
#include "Camera.h"
#include "GameObject.h"
#include "GLSL.h"
#include "ResourceManager.h"
#include <iostream>

//reference: https://learnopengl.com/In-Practice/2D-Game/Particles

unsigned int lastDeadParticle = 0;
ResourceManager* rm = ResourceManager::getInstance();

void ParticleSystem::Start()
{
  time = time->getInstance();
  color_modify_value = 1.0f;
}

void ParticleSystem::Update()
{
  unsigned int new_particles = 2;
  Player* pl = ((GameObject*)rm->getOther("player_game_object"))->getComponentByType<Player>();
//   glm::quat QplayerRot = pl->getRotation();
//   glm::vec3 playerRot = vec3(QplayerRot.x, QplayerRot.y, QplayerRot.z);
//   glm::vec3 playerPos = pl->getPosition();
//   glm::vec3 fwd = pl->getForward();

  // add new particles
  for (unsigned int i = 0; i < new_particles; ++i)
  {
      int dead = deadParticle();
      particles[dead].load(start, color, max_velocity, min_velocity, lifespan);
  }

  // update other particles
  for (unsigned int i = 0; i < numParticles; ++i)
  {
      Particle &p = particles[i];
      p.setLifespan(time->getFrametime());
      if (p.getLifespan() > 0.0f)
      {
            p.setPosition(p.getVelocity() * (float)time->getFrametime());
            points[i*3+0] =p.getPosition().x; 
            points[i*3+1] =p.getPosition().y; 
            points[i*3+2] =p.getPosition().z; 
            float incRed = p.randFloat(0.0, 4.0);
            p.setColor(vec4(p.getColor().r + incRed, p.getColor().g, p.getColor().b, (p.getColor().a*time->getFrametime())));
            vec4 col = p.getColor();
            pointColors[i*4+0] = col.r;
            pointColors[i*4+1] = col.g; 
            pointColors[i*4+2] = col.b;
            pointColors[i*4+3] = col.a;
      }
  }

   glBindBuffer(GL_ARRAY_BUFFER, vertBuffObj);
   glBufferData(GL_ARRAY_BUFFER, sizeof(points), NULL, GL_STREAM_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*numParticles*3, points);
   glBindBuffer(GL_ARRAY_BUFFER, colorbuff);
   glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), NULL, GL_STREAM_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*numParticles*4, pointColors);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
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
        pointColors[i*4+0] = color.r; 
        pointColors[i*4+1] = color.g;
        pointColors[i*4+2] = color.b;
        pointColors[i*4+3] = color.a;
        
        Particle particle = Particle(start, max_velocity, lifespan, color);
        particles.push_back(particle);
        particle.load(start, color, max_velocity, min_velocity, lifespan);
	}

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
}
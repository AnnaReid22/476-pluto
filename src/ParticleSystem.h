#pragma once
#include "Component.h"
#include "Time.h"
#include "Particle.h"
#include "Program.h"

class ParticleSystem :
    public Component
{
private:
    Time* time = 0;
    std::vector<Particle> particles;
    GLfloat points[900];
	GLfloat pointColors[1200];
    unsigned vertArrObj;
	unsigned vertBuffObj;
	unsigned colorbuff;

public:
	vec3 start;
    vec4 color;
    vec3 max_velocity;
    vec3 min_velocity;
    float lifespan;
    int numParticles;
    std::string type;
    float color_modify_value;
    ParticleSystem(GameObject* d_GameObject) : Component(d_GameObject) {};
    void Start();
    void Update();
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };
    unsigned int deadParticle();
    void GPUSetup();
    void draw(std::shared_ptr<Program> prog);
};


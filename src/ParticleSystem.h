#pragma once
#include "Component.h"
#include "Time.h"
#include "Particle.h"

class ParticleSystem :
    public Component
{
private:
    Time* time = 0;
    std::vector<Particle> particles;
	int numParticles;
	vec3 start;

public:

    ParticleSystem(GameObject* d_GameObject) : Component(d_GameObject) {};
    void Start();
    void Update();
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };
};


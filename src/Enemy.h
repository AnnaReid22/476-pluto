#pragma once
#include "Component.h"
#include "Time.h"
#include "EnemySpawner.h"

class Enemy :
    public Component
{
private:
    Time* time = 0;

    glm::vec3 genRandomTravelDir();

    float deathTime = 1;
    float timeHit = -1;

    glm::vec3 initScale = glm::vec3(0);



public:
    glm::vec3 travelDirection = glm::vec3(0);
    float speed = 3;

    EnemySpawner* spawner = nullptr;

    Enemy(GameObject* d_GameObject) : Component(d_GameObject) {};

    float getSpeedBasedOnTime();

    void Start();
    void Update();
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };

    void OnCollide(GameObject* other);
};


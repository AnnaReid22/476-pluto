#pragma once
#include "Component.h"
#include "Time.h"
#include "EnemySpawner.h"

class Enemy :
    public Component
{
private:
    float rotationSpeed = 0;
    glm::vec3 rotationAxis = glm::vec3(0, 1, 0);
    bool collided = false;
public:
    glm::vec3 travelDirection = glm::vec3(0);
    float speed = 0;
    int type = 0;
    int parent = 0;

    Enemy(GameObject* d_GameObject) : Component(d_GameObject) {};

    void Start();
    void Update();
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };

    void OnCollide(GameObject* other);
};

#pragma once
#include "Component.h"

class Physics;

class BoundingSphereCollider :
    public Component
{
public:

    glm::vec3 centerOffset = glm::vec3(0);
    float radius = 0;

    Physics* physics = nullptr;

    BoundingSphereCollider(GameObject* d_GameObject) : Component(d_GameObject) {};

    void Start();
    void Update();
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };

    void collide(GameObject* other);
};


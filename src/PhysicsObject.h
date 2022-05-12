#pragma once
#include "Time.h"
#include "GameObject.h"
#include "Component.h"
#include "World.h"
#include "ResourceManager.h"

class PhysicsObject :
    public Component
{
public:
    glm::vec3 vel = glm::vec3(0.0f);
    glm::vec3 acc = glm::vec3(0.0f);

    PhysicsObject(GameObject* d_GameObject) : Component(d_GameObject) {};

    void Start() {};
    void Update() {
        float dt = Time::getInstance()->getFrametime();
        Camera* cam = (Camera*)ResourceManager::getInstance()->getOther("activeCamera");
        vel += acc * dt;
        gameObject->transform.position += vel * dt;
        if (length(cam->pos - this->gameObject->transform.position) > 300) {
            gameObject->world->destroyObject(gameObject);
        }
    }
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };

    void OnCollide(GameObject* other) {};
};


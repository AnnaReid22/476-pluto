#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <map>
#include <string>

#include "GameObject.h"

class BoundingSphereCollider;

class Physics
{
private:
    static Physics* instance;

    glm::vec3 gravity;

    std::vector<GameObject*> collideableObjects;

    Physics() {
        gravity = glm::vec3(0, -9.8f, 0);
    }

    bool isColliding(GameObject* obj1, GameObject* obj2);

public:
    static Physics* getInstance() {
        if (!instance)
            instance = new Physics;
        return instance;
    }

    glm::vec3 getGravity() {
        return this->gravity;
    }

    void setGravity(glm::vec3 data) {
        this->gravity = data;
    }

    void registerCollideable(GameObject* obj);
    void clearCollideables();

    void checkCollisions();


    


};


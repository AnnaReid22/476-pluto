#pragma once

#include <vector>

#include "Camera.h"

class GameObject;

class World
{
public:
    Camera* mainCamera;
    std::vector<GameObject*> entities;

    World();
    virtual ~World() {};

    void handleDestructions();
    void runSimulation();

    void addObject(GameObject* entityToBeAdded);
    void destroyObject(GameObject* entity);

    std::vector<GameObject*> getRenderables();
    std::vector<GameObject*> getParticleSystems();

private:

};



#include <iostream>

#include "World.h"
#include "MeshRenderer.h"
#include "GameObject.h"

World::World()
{}

void World::addObject(GameObject* entityToBeAdded)
{
    entityToBeAdded->world = this;
    entityToBeAdded->Start();
    entities.push_back(entityToBeAdded);
}


void World::destroyObject(GameObject* entity)
{
    entity->toBeDestroyed = true;
}

std::vector<GameObject*> World::getRenderables()
{
    std::vector<GameObject*> renderables;

    for (GameObject* g : entities)
    {
        auto renderer = g->getComponentByType<MeshRenderer>();
        if (renderer != nullptr && renderer->isEnabled)
            renderables.push_back(g);
    }

    return renderables;
}

void World::handleDestructions()
{
    // This function is written in such wack fashion to avoid segfaults from a vector allocator
    std::vector<GameObject*> temp = entities;
    std::vector<GameObject*> toBeKilled;

    for (GameObject* obj : temp)
    {
        if (obj->toBeDestroyed)
            toBeKilled.push_back(obj);
    }

    for (GameObject* obj : toBeKilled)
    {
        temp.erase(std::remove(temp.begin(), temp.end(), obj), temp.end());
        delete(obj);
    }

    entities = temp;
    return;
}

void World::runSimulation()
{
    std::vector<GameObject*> temp = entities;
    // Calls on every GameObject in the world to run its update routine

    for (GameObject* g : temp)
    {
        g->Update();
    }
}

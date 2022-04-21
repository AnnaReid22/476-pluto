#pragma once

#include <vector>
#include <memory>

#include "Component.h"
#include "Time.h"
#include "World.h"
#include "Shape.h"
#include "Material.h"

class EnemySpawner :
    public Component
{
private:
    Time* time = 0;

    double lastTime = 0;

    int playerScore = 0;
    std::vector<GameObject*> enemies;

    World* world = nullptr;

    bool printedWinMessage = false;

public:

    std::shared_ptr<Shape> enemyMesh;
    std::shared_ptr<Material> enemyMat;

    double spawnDelay = .7;// 12;

    EnemySpawner(GameObject* d_GameObject) : Component(d_GameObject) {};

    double getAsteroidPosBasedOnTime();

    GameObject* generateEnemy();

    void Start();
    void Update();
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };

    void EnemyKilled(GameObject* enemy);

    void printStats();
};


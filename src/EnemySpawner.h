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
    double nextTime = 0;

    int playerScore = 0;

    int curId = 0;

    bool printedWinMessage = false;

    void spawnEnemy();

public:

    double spawnDelay = .7;

    EnemySpawner(GameObject* d_GameObject) : Component(d_GameObject) {};

    void Start() {};
    void Update();
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };
};


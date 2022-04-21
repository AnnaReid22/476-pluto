#pragma once
#include "Component.h"
#include "Camera.h"

class Player :
    public Component
{

public:

    Player(GameObject* d_GameObject) : Component(d_GameObject) {};

    void Start();
    void Update();
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };

    void OnCollide(GameObject* other);
};


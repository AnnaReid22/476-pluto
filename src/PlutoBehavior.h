#pragma once
#include "Component.h"
#include "Time.h"

class PlutoBehavior :
    public Component
{

private:
    Time* time = nullptr;

public:

    void Start();
    void Update();
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };

    PlutoBehavior(GameObject* d_GameObject) : Component(d_GameObject) {};
};


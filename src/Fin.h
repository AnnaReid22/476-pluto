#pragma once
#include "Component.h"
class Fin : public Component
{
public:
    void Start();
    void Update();
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };
    glm::vec3 originalFinPos;
};



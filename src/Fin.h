#pragma once
#include "Component.h"
#include <memory>
class Fin : public Component
{
public:
    Fin(GameObject* d_GameObject);
    void Start();
    void Update();
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };
    void SetFinVars();

    glm::vec3 originalFinPos;
    glm::vec3 finDirection;
    glm::quat originalHierarchicalFinRot;
    bool initializedVariables;
};



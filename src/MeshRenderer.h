#pragma once

#include <memory>

#include "Component.h"
#include "Shape.h"
#include "Material.h"

class MeshRenderer :
    public Component
{
public:

    void Start() {};
    void Update() {};
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };

    std::shared_ptr<Shape> mesh;
    std::shared_ptr<Material> material;

    MeshRenderer(GameObject* d_GameObject) : Component(d_GameObject) {};
};


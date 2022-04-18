#pragma once

#include <string>

//#include "GameObject.h"
#include "Transform.h"

class GameObject;

class Component
{
public:
    Component() = default;
    Component(GameObject * d_GameObject);

    virtual ~Component() {};
    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void Enable() = 0;
    virtual void Disable() = 0;

    virtual void OnCollide(GameObject* other);

    bool isEnabled = true;
    GameObject* gameObject;
    
protected:
};



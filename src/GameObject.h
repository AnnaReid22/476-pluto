#pragma once

#include <vector>
#include <string>

#include "Transform.h"

class Component;
class World;

class GameObject
{
public:
    Transform transform;
    std::string name;
    bool isEnabled = true;
    bool toBeDestroyed = false;
    bool parent = false;

    // These two variables are used to deform the rocket in the vertex shader
    bool deform;
    float deformFactor;
    GameObject* parentObj;
    std::string tag;

    World * world;

    GameObject(std::string d_Name);
    GameObject(std::string d_Name, Transform d_Transform);
    ~GameObject();

    void Start();
    void Update();
    void Enable();
    void Disable();

    void OnCollide(GameObject * other);

    // This function is only defined for types that are subtypes of Component
    // dont call it with any random type unless you are a dingus
    template<typename T>
    T* getComponentByType()
    {
        for (Component* comp : components)
        {
            if (dynamic_cast<T*>(comp) != nullptr)
                return dynamic_cast<T*>(comp);
        }

        return nullptr;
    }

    // This function is only defined for types that are subtypes of Component
    // dont call it with any random type unless you are a dingus
    template<typename T>
    T* addComponentOfType()
    {
        Component* temp;
        if (GameObject::getComponentByType<T>() == nullptr)
        {
            temp = new T(this);
            components.push_back(temp);
        }

        return dynamic_cast<T*>(temp);
    }

private:
    std::vector<Component*> components;
};




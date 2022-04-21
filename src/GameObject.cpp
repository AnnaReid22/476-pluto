#include "GameObject.h"
#include "Component.h"
#include "World.h"

#include <iostream>

GameObject::GameObject(std::string d_Name)
{
    transform = Transform();
    name = d_Name;
    tag = "";
}

GameObject::GameObject(std::string d_Name, Transform d_Transform)
{
    name = d_Name;
    transform = d_Transform;
    tag = "";
}

GameObject::~GameObject()
{
    for (Component* comp : components)
    {
        delete(comp);
    }
}

void GameObject::Start()
{
    for (Component* comp : components)
    {
        if(comp->isEnabled)
            comp->Start();
    }
}

void GameObject::Update()
{
    // If the object was marked to be destroyed, kill it before it can update
    if (this->toBeDestroyed == true)
    {
        delete(this);
        return;
    }

    for (Component* comp : components)
    {
        if(comp->isEnabled)
            comp->Update();
    }
}

void GameObject::Enable()
{
    isEnabled = true;
}

void GameObject::Disable()
{
    isEnabled = false;
}

void GameObject::OnCollide(GameObject* other)
{
    for (Component* comp : components)
    {
        if (comp->isEnabled)
            comp->OnCollide(other);
    }
}

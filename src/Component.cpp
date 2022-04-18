#include "Component.h"
#include "GameObject.h"

Component::Component(GameObject * d_GameObject)
{
    gameObject = d_GameObject;
}

void Component::OnCollide(GameObject* other)
{
}

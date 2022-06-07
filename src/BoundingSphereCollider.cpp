#include "BoundingSphereCollider.h"
#include "MeshRenderer.h"
#include "Component.h"
#include "Physics.h"
#include "GameObject.h"

void BoundingSphereCollider::Start()
{
    // Cache the instance of the physics singleton
    physics = physics->getInstance();
    
    // Attempt to get the Bounding Sphere information from a MeshRenderer
    auto mr = gameObject->getComponentByType<MeshRenderer>();
    if (mr != nullptr)
    {
        this->centerOffset = mr->mesh->getBSphere(&(this->radius));
    }
    else
    {
        this->centerOffset = glm::vec3(0, 0, 0);
        this->radius = 1.0f;
    }
}

void BoundingSphereCollider::Update()
{
    // Add self to the registry for elements to be checked for collisions
    physics->registerCollideable(this->gameObject);
}

void BoundingSphereCollider::collide(GameObject* other)
{
    gameObject->OnCollide(other);
}

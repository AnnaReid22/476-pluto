#include "Physics.h"
#include "BoundingSphereCollider.h"
#include "GameObject.h"

void Physics::registerCollideable(GameObject* obj)
{
    collideableObjects.push_back(obj);
}

bool Physics::isColliding(GameObject* obj1, GameObject* obj2)
{
    glm::vec3 p1, p2;

    BoundingSphereCollider* col1;
    BoundingSphereCollider* col2;

    p1 = obj1->transform.position;
    p2 = obj2->transform.position;

    col1 = obj1->getComponentByType<BoundingSphereCollider>();
    col2 = obj2->getComponentByType <BoundingSphereCollider>();

    if (col1 == nullptr || col2 == nullptr)
        return false;

    glm::vec3 center1 = p1 + col1->centerOffset;
    glm::vec3 center2 = p2 + col2->centerOffset;

    return glm::distance(center1, center2) <= (col1->radius + col2->radius);
}

void Physics::clearCollideables()
{
    collideableObjects.clear();
}

void Physics::checkCollisions()
{
    for (int i = 0; i < collideableObjects.size(); i++)
    {
        for (int j = 0; j < collideableObjects.size(); j++)
        {
            // Dont compare collisions with yourself
            if (i == j) continue;

            if (isColliding(collideableObjects[i], collideableObjects[j]))
            {
                BoundingSphereCollider * col = collideableObjects[i]->getComponentByType<BoundingSphereCollider>();
                col->collide(collideableObjects[j]);
            }
        }
    }
}

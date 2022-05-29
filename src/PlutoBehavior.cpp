#include "PlutoBehavior.h"
#include "GameObject.h"

void PlutoBehavior::Start()
{
    time = time->getInstance();
}

void PlutoBehavior::Update()
{
    double currTime = time->getGlobalTime();
    this->gameObject->transform.scale = glm::vec3(15) * (float)(glm::sin(currTime * 2.f) * 0.25 + 1.25f);
}
#include <random>

#include "Enemy.h"
#include "GameObject.h"

#include "BoundingSphereCollider.h"

#include <iostream>

glm::vec3 Enemy::genRandomTravelDir()
{
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0, 2.0);

    glm::vec3 dir = glm::vec3(0, 0, 1);
    dir = glm::normalize(dir);

    return dir;
}

/*
Gets speed of instantiated asteroid based on time since the start of the game. The more time has passed, 
the faster the asteroids move.
*/
float Enemy::getSpeedBasedOnTime()
{
    Time* time = time->getInstance();
    double secSinceStart = time->getGlobalTime();
    return  (secSinceStart+2)/5.0f;
}

void Enemy::Start()
{
    time = time->getInstance();

    travelDirection = genRandomTravelDir();

    speed = getSpeedBasedOnTime();

    initScale = this->gameObject->transform.scale;
}

void Enemy::Update()
{
    // Destroy enemies if they reached beginning of level
    if (this->gameObject->transform.position.z > 100)
    {
        spawner->EnemyKilled(this->gameObject);
    }
    // Check if this enemy collided with something
    if (timeHit > 0)
    {
        float t = 1 - (((timeHit + deathTime) - time->getGlobalTime()) / deathTime);

        if (t >= 1)
        {
            spawner->EnemyKilled(this->gameObject);
        }
        else
        {
            this->gameObject->transform.scale = glm::mix(initScale, glm::vec3(0.01, 0.01, 0.01), t);
        }

        return;
    }
    this->gameObject->transform.position = this->gameObject->transform.position + (speed * float(time->getFrametime()) * travelDirection);
    if (this->gameObject->transform.position.x > 14 || this->gameObject->transform.position.x < -14)
        travelDirection = genRandomTravelDir();
    if (this->gameObject->transform.position.z > 14 || this->gameObject->transform.position.z < -14)
        travelDirection = genRandomTravelDir();

    glm::quat q;
    glm::vec3 a = glm::cross(glm::vec3(0, 0, 1), travelDirection);
    q.x = a.x;
    q.y = a.y;
    q.z = a.z;
    q.w = 1 + glm::dot(glm::vec3(0, 0, 1), travelDirection);

    q = glm::normalize(q);

    this->gameObject->transform.rotation = q;
}


void Enemy::OnCollide(GameObject* other) 
{
    if (other->name == "player")
    {
        BoundingSphereCollider* bsc = this->gameObject->getComponentByType<BoundingSphereCollider>();
        if (bsc != nullptr && bsc->isEnabled)
            bsc->Disable();

        speed = 0;

        timeHit = time->getGlobalTime();
    }
    else if (other->name == "dog")
    {
        travelDirection = genRandomTravelDir();
    }
}

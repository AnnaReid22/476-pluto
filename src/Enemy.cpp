#include <random>

#include "Enemy.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"
#include "BoundingSphereCollider.h"

#include <iostream>

void Enemy::Start()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1, 1);

    rotationSpeed = dist(gen);
    rotationAxis = glm::normalize(glm::vec3(dist(gen), dist(gen), dist(gen)));

    if (type == 0) {
        travelDirection = glm::normalize(glm::vec3(dist(gen), dist(gen), 5)) * 3.f;
        //speed = (Time::getInstance()->getGlobalTime() + 2) / 5.0f;
        speed = 1;
    }
}

void Enemy::Update()
{
    this->cooldown -= Time::getInstance()->getFrametime();
    ResourceManager* rm = ResourceManager::getInstance();
    glm::vec3 campos = ((Camera*)rm->getOther("activeCamera"))->pos;
    if (glm::length(gameObject->transform.position - campos) > 250.0f)
    {
        gameObject->toBeDestroyed = true;
    }
    float dt = Time::getInstance()->getFrametime();
    float r = dt * rotationSpeed * 0.5;
    glm::vec3 sine = rotationAxis * sin(r);
    gameObject->transform.rotation *= glm::quat(cos(r), sine.x, sine.y, sine.z);
    gameObject->transform.position += travelDirection * speed * dt;

    if (collided) {
        collided = false;
        if (this->type < 7) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dist(-1, 1);

            glm::vec3 offVec1 = normalize(glm::vec3(dist(gen), dist(gen), 0));
            glm::vec3 offVec2 = glm::vec3(-offVec1.x, -offVec1.y, 0);

            std::shared_ptr<Shape>* asteroid_shapes = (std::shared_ptr<Shape>*) (rm->getOther("asteroid_shapes"));
            std::shared_ptr<Material> asteroid_material = *(std::shared_ptr<Material> *) rm->getOther("asteroid_material");

            GameObject* child1 = new GameObject("asteroid");
            child1->transform.position = gameObject->transform.position;
            child1->transform.scale = gameObject->transform.scale;
            child1->transform.rotation = gameObject->transform.rotation;
            Enemy* enemy1 = child1->addComponentOfType<Enemy>();
            enemy1->type = 2 * type + 1;
            enemy1->travelDirection = normalize(travelDirection + offVec1);
            enemy1->speed = speed + 1;
            enemy1->cooldown = 2;
            MeshRenderer* renderer1 = child1->addComponentOfType<MeshRenderer>();
            renderer1->mesh = asteroid_shapes[enemy1->type];
            renderer1->material = asteroid_material;
            BoundingSphereCollider* bsc1 = child1->addComponentOfType<BoundingSphereCollider>();
            bsc1->radius = 0.65;
        
            GameObject* child2 = new GameObject("asteroid");
            child2->transform.position = gameObject->transform.position;
            child2->transform.scale = gameObject->transform.scale;
            child2->transform.rotation = gameObject->transform.rotation;
            Enemy* enemy2 = child2->addComponentOfType<Enemy>();
            enemy2->type = 2 * type + 2;
            enemy2->travelDirection = normalize(travelDirection + offVec2);
            enemy2->speed = speed + 1;
            enemy2->cooldown = 2;
            MeshRenderer* renderer2 = child2->addComponentOfType<MeshRenderer>();
            renderer2->mesh = asteroid_shapes[enemy2->type];
            renderer2->material = asteroid_material;
            BoundingSphereCollider* bsc2 = child2->addComponentOfType<BoundingSphereCollider>();
            bsc2->radius = 0.65;

            child1->transform.position += enemy1->travelDirection;
            child2->transform.position += enemy2->travelDirection;

            gameObject->world->destroyObject(gameObject);
            gameObject->world->addObject(child1);
            gameObject->world->addObject(child2);

        }
        else 
        {
            this->gameObject->world->destroyObject(gameObject);
        }
    }
}


void Enemy::OnCollide(GameObject* other) 
{
    if (this->cooldown < 0) {
        collided = true;
    }
}

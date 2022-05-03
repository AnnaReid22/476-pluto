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
        speed = (Time::getInstance()->getGlobalTime() + 2) / 5.0f;
    }
}

void Enemy::Update()
{
    if (collided) {
        collided = false;
        if (this->type < 7) {
            glm::vec3 offsetDir = glm::vec3(3, 1, 0);

            ResourceManager* rm = ResourceManager::getInstance();
            std::shared_ptr<Shape>* asteroid_shapes = (std::shared_ptr<Shape>*) (rm->getOther("asteroid_shapes"));
            std::shared_ptr<Material> asteroid_material = *(std::shared_ptr<Material> *) rm->getOther("asteroid_material");

            GameObject* child1 = new GameObject("asteroid");
            child1->transform.position = gameObject->transform.position + glm::vec3(1, 0, -1);
            child1->transform.scale = gameObject->transform.scale;
            child1->transform.rotation = gameObject->transform.rotation;
            Enemy* enemy1 = child1->addComponentOfType<Enemy>();
            enemy1->type = 2 * type + 1;
            enemy1->travelDirection = normalize(travelDirection + offsetDir);
            enemy1->speed = speed;
            enemy1->parent = parent;
            MeshRenderer* renderer1 = child1->addComponentOfType<MeshRenderer>();
            renderer1->mesh = asteroid_shapes[enemy1->type];
            renderer1->material = asteroid_material;
            BoundingSphereCollider* bsc1 = child1->addComponentOfType<BoundingSphereCollider>();
            bsc1->radius = 0.65;

        
            GameObject* child2 = new GameObject("asteroid");
            child2->transform.position = gameObject->transform.position + glm::vec3(-1, 0, -1);
            child2->transform.scale = gameObject->transform.scale;
            child2->transform.rotation = gameObject->transform.rotation;
            Enemy* enemy2 = child2->addComponentOfType<Enemy>();
            enemy2->type = 2 * type + 2;
            enemy2->travelDirection = normalize(travelDirection - offsetDir);
            enemy2->speed = speed;
            enemy2->parent = parent;
            MeshRenderer* renderer2 = child2->addComponentOfType<MeshRenderer>();
            renderer2->mesh = asteroid_shapes[enemy2->type];
            renderer2->material = asteroid_material;
            BoundingSphereCollider* bsc2 = child2->addComponentOfType<BoundingSphereCollider>();
            bsc2->radius = 0.65;
        

            gameObject->world->destroyObject(gameObject);
            gameObject->world->addObject(child1);
            gameObject->world->addObject(child2);

        }
        else 
        {
            this->gameObject->world->destroyObject(gameObject);
        }
    }
    else {

        // Destroy enemies if they reached beginning of level
        if (gameObject->transform.position.z > 100)
        {
            gameObject->toBeDestroyed = true;
        }
        float dt = Time::getInstance()->getFrametime();
        float r = dt * rotationSpeed * 0.5;
        glm::vec3 sine = rotationAxis * sin(r);
        gameObject->transform.rotation *= glm::quat(cos(r), sine.x, sine.y, sine.z);
        gameObject->transform.position += travelDirection * speed * dt;
        if (type != 0) {
            std::cout << "not0: " << speed << std::endl;
        }
    }
}


void Enemy::OnCollide(GameObject* other) 
{

    if (other->name == "asteroid")
    {
        Enemy* e = other->getComponentByType<Enemy>();
        if (e->parent != this->parent) {
            collided = true;
        }
    }
    else if (other->name == "player")
    {
        collided = true;
    }
}

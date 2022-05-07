#include <iostream>
#include <random>

#include "EnemySpawner.h"
#include "GameObject.h"

#include "MeshRenderer.h"
#include "BoundingSphereCollider.h"
#include "Enemy.h"
#include "ResourceManager.h"

void EnemySpawner::Update()
{
	float time = Time::getInstance()->getGlobalTime();
	if (time >= nextTime)
	{
			nextTime += spawnDelay;
			spawnDelay *= 0.999;
			spawnEnemy();
	}
}

void EnemySpawner::spawnEnemy()
{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dist(-1, 1);

		ResourceManager* rm = ResourceManager::getInstance();
		std::shared_ptr<Shape>* asteroid_shapes = (std::shared_ptr<Shape>*) (rm->getOther("asteroid_shapes"));
		std::shared_ptr<Material> asteroid_material = *(std::shared_ptr<Material> *) rm->getOther("asteroid_material");

		glm::vec3 playerPos = ((GameObject*)rm->getOther("player_game_object"))->transform.position;

		glm::vec3 startPos = glm::vec3(36 * dist(gen), 20 * dist(gen), -215) + playerPos;


		GameObject* asteroid = new GameObject("asteroid");
		asteroid->transform.position = startPos;
		asteroid->transform.scale = glm::vec3(3);
		Enemy* enemy1 = asteroid->addComponentOfType<Enemy>();
		enemy1->type = 0;
		enemy1->parent = curId++;
		MeshRenderer* renderer1 = asteroid->addComponentOfType<MeshRenderer>();
		renderer1->mesh = asteroid_shapes[0];
		renderer1->material = asteroid_material;
		BoundingSphereCollider* bsc1 = asteroid->addComponentOfType<BoundingSphereCollider>();
		// bsc1->radius = 0.65;


		gameObject->world->addObject(asteroid);
}



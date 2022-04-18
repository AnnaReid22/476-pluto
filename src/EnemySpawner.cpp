#include <iostream>
#include <random>

#include "EnemySpawner.h"
#include "GameObject.h"

#include "MeshRenderer.h"
#include "BoundingSphereCollider.h"
#include "Enemy.h"

GameObject* EnemySpawner::generateEnemy()
{
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-14.0, 14.0);
	// Create a dog gameobject with a meshrenderer and add it to the world
	GameObject* dog = new GameObject("dog");
	dog->transform.position = glm::vec3(dis(gen), 0, dis(gen));
	dog->transform.scale = glm::vec3(0.25, 0.25, 0.25);

	Enemy* enemy = dog->addComponentOfType<Enemy>();
	enemy->spawner = this;

	MeshRenderer* mr = dog->addComponentOfType<MeshRenderer>();
	mr->mesh = enemyMesh;
	mr->material = enemyMat;

	BoundingSphereCollider* bsc1 = dog->addComponentOfType<BoundingSphereCollider>();
	bsc1->radius = 1;

	return dog;
}

void EnemySpawner::Start()
{
    // Cache time
    time = time->getInstance();

    // Cache world
    world = this->gameObject->world;
}

void EnemySpawner::Update()
{
	if (lastTime > 0 && enemies.size() == 0)
	{
		if (!printedWinMessage)
		{
			std::cout << "You win! Score was: " << playerScore << std::endl;
			printedWinMessage = true;
		}
		return;
	}

	if (time->getGlobalTime() >= lastTime + spawnDelay)
	{
		std::cout << "Too slow! Adding another enemy..." << std::endl;
		GameObject* enemy = generateEnemy();

		world->addObject(enemy);
		enemies.push_back(enemy);

		BoundingSphereCollider* bsc = enemy->getComponentByType<BoundingSphereCollider>();
		bsc->radius = 1;

		printStats();

		lastTime = time->getGlobalTime();
	}
}

void EnemySpawner::EnemyKilled(GameObject* enemy)
{
	std::cout << "You got one!" << std::endl;

	enemies.erase(std::remove(enemies.begin(), enemies.end(), enemy), enemies.end());
	playerScore++;

	world->destroyObject(enemy);
	printStats();
}

void EnemySpawner::printStats()
{
	std::cout << "# of Enemies: " << enemies.size() << std::endl;
	std::cout << "Current Score: " << playerScore << std::endl;
}



#include <iostream>
#include <random>

#include "EnemySpawner.h"
#include "GameObject.h"

#include "MeshRenderer.h"
#include "BoundingSphereCollider.h"
#include "Enemy.h"

double EnemySpawner::getAsteroidPosBasedOnTime()
{
	Time* time = time->getInstance();
	double secSinceStart = time->getGlobalTime();
	glm::vec3 camPos = this->gameObject->world->mainCamera->pos;
	std::cout << "asteroid pos: " << camPos.z - 205 << std::endl;
	// Spawn asteroids just outside the far plane
	return camPos.z - 205;
	//alternative based on time rather than camera position max(-1000, - 10 * secSinceStart - 200);
}
GameObject* EnemySpawner::generateEnemy()
{
	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> disY(-15.0, 15.0);
	std::uniform_real_distribution<float> disX(-15.0, 15.0);
	// Create a dog gameobject with a meshrenderer and add it to the world
	GameObject* asteroid = new GameObject("asteroid");

	// Determine position to spawn asteroid
	float posX = disX(gen);
	float posY = disY(gen);
	std::cout << "posX: " << posX << ", posY: " << posY << std::endl;
	asteroid->transform.position = glm::vec3(posX, posY, getAsteroidPosBasedOnTime());
	asteroid->transform.scale = glm::vec3(0.8, 0.8, 0.8);

	Enemy* enemy = asteroid->addComponentOfType<Enemy>();
	enemy->spawner = this;

	MeshRenderer* mr = asteroid->addComponentOfType<MeshRenderer>();
	mr->mesh = enemyMesh;
	mr->material = enemyMat;

	BoundingSphereCollider* bsc1 = asteroid->addComponentOfType<BoundingSphereCollider>();
	bsc1->radius = 1;

	return asteroid;
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
	/*
	if (lastTime > 0 && enemies.size() == 0)
	{
		if (!printedWinMessage)
		{
			std::cout << "You win! Score was: " << playerScore << std::endl;
			printedWinMessage = true;
		}
		return;
	}*/


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



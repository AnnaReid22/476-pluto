#include "Fin.h"
#include "Component.h"
#include "GameObject.h"
Fin::Fin(GameObject* d_GameObject) : Component(d_GameObject)
{

}
void Fin::Start()
{
	originalFinPos = glm::vec3(0);
	finDirection = glm::vec3(0);
	originalHierarchicalFinRot = glm::quat(1.0, 0.0, 0.0, 0.0);
	initializedVariables = false;
}
void Fin::Update()
{

}
void Fin::SetFinVars()
{
	GameObject* finObj = this->gameObject;
	originalFinPos = glm::vec3(finObj->transform.getHierarchicalRot1Matrix() * glm::vec4(finObj->transform.position, 1.0f));
	finDirection = glm::vec3(7.0f*originalFinPos.x, originalFinPos.y, 2.0f*originalFinPos.z);
	originalHierarchicalFinRot = finObj->transform.hierarchicalRot1;
	initializedVariables = true;
}

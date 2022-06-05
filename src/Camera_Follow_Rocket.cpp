#include "Camera_Follow_Rocket.h"
#include "Camera.h";
#include "Component.h"
#include "GameObject.h"
#include "Camera.h"

Camera_Follow_Rocket::Camera_Follow_Rocket(GameObject* d_GameObject) : Component(d_GameObject)
{
	camDistFromTarget = 3.0f;
	startPanning = false;
	startWinAnimation = false;
}

void Camera_Follow_Rocket::Start() 
{
	camera = this->gameObject->getComponentByType<Camera>();
}
void Camera_Follow_Rocket::Update()
{
	float f = 0.08;
	camera->pos =  camera->pos + ((rocket->getPosition() + rocket->getForward() * camDistFromTarget + rocket->getUpVector() * camDistFromTarget / 2.0f) - camera->pos) * f;
	camera->target = rocket->getPosition();
	if (startPanning)
	{
		camDistFromTarget += 0.04;
		camera->target += rocket->getForward() * (camDistFromTarget -3.0f)*0.5f;
		//camera->target += finObj->transform.hierarchicalTrans2 + glm::vec3(glm::mat4(rotationQuat) * glm::vec4(finObj->transform.position,1));// (finObj->transform.position + finObj->transform.hierarchicalTrans2);
	}
	if (startWinAnimation)
	{
		camDistFromTarget += 0.04;
		camera->pos = glm::vec3(winPos.x, winPos.y, winPos.z + (camDistFromTarget-3.0f));
	}
	
	
}

void Camera_Follow_Rocket::SetUpLoseFin(GameObject* fin)
{
	rotationQuat = fin->transform.hierarchicalRot1;
	camDistFromTarget = 3.0f;
	startPanning = true;
	finObj = fin;
}

void Camera_Follow_Rocket::StopPanning()
{
	camDistFromTarget = 3.0f;
	startPanning = false;
	camera->target = rocket->getPosition();
}

void Camera_Follow_Rocket::SetUpWinAnimation()
{
	camDistFromTarget = 3.0f;
	startWinAnimation = true;
	winPos = camera->pos;
}

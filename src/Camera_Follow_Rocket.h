#pragma once
#include "Component.h"
#include "Player.h"
#include "Camera.h"
#include "GameObject.h"
class Camera_Follow_Rocket :
    public Component
{
private:
    float camDistFromTarget;
    Camera* camera;
    GameObject* finObj;
    glm::quat rotationQuat;
    glm::vec3 winPos;
 

public:
    Player* rocket; 
    bool startPanning;
    bool startWinAnimation;

    Camera_Follow_Rocket(GameObject* d_GameObject);
    void Start();
    void Update();
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };
    void SetUpLoseFin();
    void StopPanning();
    void SetUpWinAnimation();

};


#pragma once

#include "WindowManager.h"
#include "Component.h"
#include <glm/gtc/type_ptr.hpp>
#include "InputManager.h"
//#include "Player.h"

class Camera : public Component
{
private:
    float camDist;
    bool swapViewChoice;
    WindowManager* windowManager = nullptr;

public:
    //glm::vec3 camPos;
    glm::vec3 target;
    glm::vec3 upVector;


    glm::vec3 pos;
    glm::vec3 pos2;

    //Player* rocket;

    Camera(GameObject* d_GameObject);
    void setUpCam();

    void Start() {};
    void Update();
    void Enable() {};
    void Disable() {};

    glm::mat4 getCameraViewMatrix();
    glm::mat4 getCameraProjectionMatrix();
    glm::mat4 getCameraRotationMatrix();
};


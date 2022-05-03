#pragma once

#include "WindowManager.h"
#include "Component.h"
#include <glm/gtc/type_ptr.hpp>
#include "InputManager.h"
#include "Player.h"

class Camera : public Component
{
private:
    float camDist;

public:
    glm::vec3 upVector;
    glm::vec3 pos;

    Player* rocket;

    Camera(GameObject* d_GameObject);
    void setUpCam();

    void Start() {};
    void Update();
    void Enable() {};
    void Disable() {};

    glm::mat4 getCameraViewMatrix();
    glm::mat4 getCameraRotationMatrix();
};


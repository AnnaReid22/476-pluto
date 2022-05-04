#pragma once

#include "WindowManager.h"
#include "Component.h"
#include <glm/gtc/type_ptr.hpp>
#include "InputManager.h"

class Camera : public Component
{
private:
    float phi;
    float theta;
    int lastX;
    int lastY;
    float speed;
    float sensitivity;
    bool firstMouse;

    InputManager* input;

    void updateMoveVars();
    void moveLookAt(WindowManager* windowManager);
    void moveEye();

public:
    glm::vec3 eyeOffset;
    glm::vec3 lookAt;
    glm::vec3 upVector;
    glm::vec3 gaze;

    bool dollyF;
    bool dollyB;
    bool strafeR;
    bool strafeL;
    bool rise;
    bool fall;
    bool stop;

    WindowManager* windowManager;

    Camera(GameObject* d_GameObject);
    void setUpCam(WindowManager* windowManager);
    void setEyePos(glm::vec3 pos);
    void setSpeed(float s);
    void setSens(float sns);
    void enableCursor(WindowManager* windowManager);

    float getTheta() { return theta; }
    float getPhi() { return phi; }

    void Start() {};
    void Update();
    void Enable() {};
    void Disable() {};

    glm::mat4 getCameraViewMatrix();
    glm::mat4 getCameraProjectionMatrix();
    glm::mat4 getCameraRotationMatrix();
};


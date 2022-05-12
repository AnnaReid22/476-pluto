#pragma once
#include "WindowManager.h"
#include "Component.h"
#include "InputManager.h"

class Player : public Component
{
private:
    InputManager* input;

    // Rocket rotation variables
    double currentXPos, currentYPos, initXPos, initYPos;
    double deltaXPos, deltaYPos;
    float sensitivity;
    glm::vec3 rotation, fwd;
    
    float speed;
    float bulletCooldown;

    // Rocket movement vectors
    void updateMoveVars();
    void moveRocket();

public:
    // Movement variables
    bool dollyF;
    bool dollyB;
    bool stop;
    bool shoot;

    // Rotation variables
    bool initMousePosSet;


    Player(GameObject* d_GameObject);

    void Start();
    void Update();
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };

    void OnCollide(GameObject* other);

    // Rocket accessor methods
    glm::vec3 getPosition();
    glm::vec3 getForward();
    glm::quat getRotation();
    float getXRotation();
};


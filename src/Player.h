#pragma once
#include "WindowManager.h"
#include "Component.h"
#include "InputManager.h"
#include "Shape.h"

class Player : public Component
{
private:
    InputManager* input;

    // Rocket rotation variables
    double currentXPos, currentYPos, initXPos, initYPos;
    double deltaXPos, deltaYPos;
    float sensitivity;
    glm::vec3 rotation, fwd, originalScale;
    
    float speed;
    float bulletCooldown;
    float stopTime;
    float prepareShootTime;
    bool alreadyShot;

    // Rocket movement vectors
    void updateMoveVars();
    void moveRocket();

public:
    // Movement variables
    bool dollyF;
    bool dollyB;
    float dollyFTime;
    bool prevDollyF;
    bool stop;
    bool shoot;
    bool dead;
    float shakeTime;
    float collideTime;
    glm::mat4 rotMat;
    Time* time;
    glm::vec4 posUpdate;

    // Rotation variables
    bool initMousePosSet;


    Player(GameObject* d_GameObject);

    void Start();
    void Update();
    void Enable() { this->isEnabled = true; };
    void Disable() { this->isEnabled = false; };

    void OnCollide(GameObject* other);
    void KillRocket();
    void ShakeRocket();
    void DisassembleRocket();
    void PrepareShoot();
    void Shoot();

    // Rocket accessor methods
    glm::vec3 getPosition();
    glm::vec3 getForward();
    glm::quat getRotation();
    glm::vec3 getRightVector();
    glm::vec3 getUpVector();

    float getXRotation();

    // Rockets fin mesh
    std::shared_ptr<Shape> finMesh;

    GameObject* rocketBody;
    GameObject* fin1;
    GameObject* fin2;
    GameObject* fin3;
};


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

    // Variable used to trigger losing fins 
   // bool loseFins;
    float loseFinsTime;

    // Variables used to give rocket a new set of fins
    //bool getFins;
    float getFinsTime;

    glm::vec3 rotation, fwd, originalScale;
    
    float speed;
    float bulletCooldown;
    float stopTime;
    float prepareShootTime;
    bool alreadyShot;
    float wonTimer;
    // Rocket movement vectors
    void updateMoveVars();
    void moveRocket();


public:
    // How many asteroids can hit you before you die.
    int numLives;
    // Movement variables
    bool dollyF;
    bool dollyB;
    float dollyFTime;
    bool prevDollyF;
    bool stop;
    bool shoot;
    bool dead;
    bool won;
    bool setOriginalDisassemblePositions;
    float shakeTime;
    float collideTime;
    glm::mat4 rotMat;
    Time* time;
    glm::vec4 posUpdate;
    //glm::vec3 originalFin1Pos;
    //glm::vec3 finDirection;
    //glm::quat originalHierarchicalFin1Rot;

    //glm::vec3 originalFin2Pos;
    //glm::quat originalHierarchicalFin2Rot;

   // glm::vec3 originalFin3Pos;
    //glm::quat originalHierarchicalFin3Rot;

    glm::vec3 originalRocketBodyPos;

    // Rotation matrices used for disassembling rocket
    glm::mat4 fin1R;

    glm::vec3 originalPlayer;
    //bool setOriginalFinPositions;

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
    void LoseFin(int finNum);
    void KillFin(int finNum);
    void WinAnimation();


    // Helper functions for DisassembleRocket
    float hash(float n);
    glm::vec3 snoiseRotation(glm::vec3 x);
    glm::mat4 rotationMatrix(glm::vec3 axis, float angle);

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
    GameObject* finObjs[3] = {NULL, NULL, NULL};
    GameObject* fin0;
    GameObject* fin1;
    GameObject* fin2;
};


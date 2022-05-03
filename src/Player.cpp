#include "GameObject.h"
#include "Player.h"
#include <iostream>



Player::Player(GameObject* d_GameObject) : Component(d_GameObject)
{
    // Has rocket collided with planet?
    stop = false;

    // Move forward and backwards
    dollyB = false;
    dollyF = false;

    // Move left and right
    strafeR = false;
    strafeL = false;

    // Move up and down
    rise = false;
    fall = false;

    // Speed for any of the above movements
    speed = -1000.0f;

    // Rocket rotation sensitivity
    sensitivity = 0.1;

    // Change in mouse position - used to calculate rocket rotation
    deltaXPos = deltaYPos = 0; 

    // Whether the initial mouse position has been set
    initMousePosSet = false;

    // Initial and current X and Y mouse positions
    initXPos = initYPos = currentXPos = currentYPos = 0;

    // Initial rocket rotation
    rotation = glm::vec3(0, -3.14159265/2.0f, 0);

    // Instance of InputManager
    input = input->getInstance();
}

/*
* Sets initial rocket position
*/
void Player::Start()
{
    this->gameObject->transform.position = glm::vec3(0, 0, -4);
}

/*
* Calls functions that move and rotate rocket
*/
void Player::Update()
{
    updateMoveVars();
    if (!stop)
    {
        moveRocket();
    }
}


/*
* Rocket stops moving when it collides with a planet
*/
void Player::OnCollide(GameObject* other) 
{
    if (other->tag == "planet")
    {
        stop = true;
    }
}


/*
* Updates variables used for rocket movement and rotation
*/
void Player::updateMoveVars()
{
    double xpos, ypos;
    
    input->GetMousePos(&xpos, &ypos);

    // Initialize mouse position 
    if (!initMousePosSet)
    {
        initMousePosSet = true;
        initXPos = xpos;
        initYPos = ypos;
    }

    // Update current mouse position
    currentXPos = xpos;
    currentYPos = ypos; 

    // Change in mouse position since last call
    deltaXPos = currentXPos - initXPos;
    deltaYPos = currentYPos - initYPos;

    // Update previous mouse position
    initXPos = currentXPos;
    initYPos = currentYPos;
    
    // Update movement variables
    dollyF = input->GetKey(GLFW_KEY_W);
    dollyB = input->GetKey(GLFW_KEY_S);
    strafeL = input->GetKey(GLFW_KEY_A);
    strafeR = input->GetKey(GLFW_KEY_D);
    rise = input->GetKey(GLFW_KEY_SPACE);
    fall = input->GetKey(GLFW_KEY_LEFT_SHIFT);
}

/*
* Calculates the rocket's position matrix and rotation quaternion and 
* updates the rocket's transform.
*/
void Player::moveRocket()
{
    Time* time = time->getInstance();
    float frametime = time->getFrametime();

    // Determine direction of rocket movement
    glm::vec3 rocketMove = glm::vec3(0.0f, 0.0f, 0.0f);
    if (dollyF)
    {
        rocketMove.y += frametime * speed;
    }
    if (dollyB)
    {
        rocketMove.y -= frametime * speed;
    }
    if (strafeR)
    {
        rocketMove.x += frametime * speed;
    }
    if (strafeL)
    {
        rocketMove.x -= frametime * speed;
    }
    if (rise)
    {
        rocketMove.z -= frametime * speed;
    }
    if (fall)
    {
        rocketMove.z += frametime * speed;
    }

    // Update rocket rotation
    rotation.z += deltaXPos * frametime * sensitivity;
    rotation.y += deltaYPos * frametime * sensitivity;

    // Used to calculate position
    glm::mat4 rotMat = glm::rotate(glm::mat4(1.f), rotation.x, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.f), rotation.y, glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.f), rotation.z, glm::vec3(0, 0, 1));
    
    // Adjust movement based on the rocket's rotation
    glm::vec4 posUpdate = rotMat * glm::vec4(rocketMove, 0); //glm::vec4(0, -10, 0, 0) if you want constant forward movement
    
    // Store fwd vector of the rocket
    fwd = normalize(rotMat * glm::vec4(0, -1, 0, 0));

    // Update the Rocket's Transform position matrix and rotation quaternion
    this->gameObject->transform.position -= glm::vec3(posUpdate.x, posUpdate.y, posUpdate.z) * 0.001f;
    this->gameObject->transform.rotation = glm::quat(rotMat);
}

// Returns rocket's position
glm::vec3 Player::getPosition()
{
    return this->gameObject->transform.position;
}

// Returns rocket's fwd vector
glm::vec3 Player::getForward()
{
    return fwd;
}

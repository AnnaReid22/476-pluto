#include "Camera.h"
#include "Time.h"
#include "GameObject.h"
#include "ResourceManager.h"
#define PI 3.14159265


#include <iostream>

Camera::Camera(GameObject* d_GameObject): Component(d_GameObject)
{
    // Distance between camera and rocket
    camDist = 3.0f;

    upVector = glm::vec3(0, 1, 0);

    // Current camera position
    pos = glm::vec3(0.f);
    pos2 = glm::vec3(0.f);

    ResourceManager* rm = rm->getInstance();
    windowManager = (WindowManager*)rm->getOther("WindowManager");
    swapViewChoice = false;
}

/*
* Updates camera position based on spring effect
*/
void Camera::setUpCam()
{
    float f = 0.08;//0.01;
    pos = pos + ((rocket->getPosition() + rocket->getForward() * camDist + rocket->getUpVector()  * camDist/2.0f) - pos) * f;
    pos2 = pos2 + ((rocket->getPosition() - rocket->getForward() * camDist + rocket->getUpVector() * camDist / -2.0f + rocket->getRightVector()* camDist / -2.0f) - pos2) * f;

}


void Camera::Update()
{
    setUpCam();
    if (InputManager::getInstance()->GetKey(GLFW_KEY_O))
    {
        swapViewChoice = false;
    }
    if (InputManager::getInstance()->GetKey(GLFW_KEY_L))
    {
        swapViewChoice = true;
    }
    std::cout << "swapViewChoice: " << swapViewChoice << std::endl;
    //upVector = glm::vec4(0, 1, 0, 0) * glm::rotate(glm::mat4(1.f), rocket->getXRotation(), glm::vec3(1, 0, 0));

}

/*
* Create View Matrix
*/
glm::mat4 Camera::getCameraViewMatrix()
{
    glm::vec3 rocketPos = rocket->getPosition();
    glm::vec3 rocketFwd = rocket->getForward();
    glm::vec3 posAbove = rocket->getPosition() + rocket->getForward() * camDist;
    //static glm::vec3 leftVector = -1.0f * rocket->getRightVector()
    static glm::vec3 posLeft = rocket->getPosition() + -1.0f * rocket->getRightVector() * 6.0f;
    static glm::vec3 fwd = rocket->getForward();
    posAbove = glm::vec3(posAbove.x, posAbove.y + 3.0f, posAbove.z + 3.0f);
    if (swapViewChoice == false)
    {
        return glm::lookAt(pos, rocketPos, upVector);
    }
    else
    {
        //return glm::lookAt(pos2, rocketPos, upVector);

        return glm::lookAt(posLeft, rocketPos, upVector);
        //return glm::lookAt(rocketPos + fwd*6.0f, rocketPos, upVector);
    }
    
}

glm::mat4 Camera::getCameraProjectionMatrix()
{
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    float aspect = width / (float)height;
    glm::mat4 p = glm::perspective(glm::radians(50.0f), aspect, 0.01f, 1000.0f);
    return p;
}

/*
* Matrix used for skybox. Does not take into account position, only rotation
*/
glm::mat4 Camera::getCameraRotationMatrix()
{
    return glm::mat4(1);// glm::lookAt(glm::vec3(0), rocket->getPosition() - pos, upVector);
}
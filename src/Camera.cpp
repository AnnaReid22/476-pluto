#include "Camera.h"
#include "Time.h"
#include "GameObject.h"
#include "ResourceManager.h"
#define PI 3.14159265


#include <iostream>

Camera::Camera(GameObject* d_GameObject): Component(d_GameObject)
{
    pos = glm::vec3(0, 0, 0);
    target = glm::vec3(0, 0, 0);
    upVector = glm::vec3(0, 1, 0);

    // Distance between camera and rocket
    camDist = 3.0f;

    ResourceManager* rm = rm->getInstance();
    windowManager = (WindowManager*)rm->getOther("WindowManager");
}

/*
* Updates camera position based on spring effect
*/
void Camera::setUpCam()
{
    float f = 0.08;//0.01;
}


void Camera::Update()
{
    setUpCam();
 }

/*
* Create View Matrix
*/
glm::mat4 Camera::getCameraViewMatrix()
{
    return glm::lookAt(pos, target, upVector);  
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
    return glm::lookAt(glm::vec3(0), target - pos, upVector);
}
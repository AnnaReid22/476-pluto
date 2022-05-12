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

    ResourceManager* rm = rm->getInstance();
    windowManager = (WindowManager*)rm->getOther("WindowManager");
}

/*
* Updates camera position based on spring effect
*/
void Camera::setUpCam()
{
    float f = 0.08;//0.01;
    pos = pos + ((rocket->getPosition() + rocket->getForward() * camDist) - pos) * f;
}


void Camera::Update()
{
    setUpCam();
    //upVector = glm::vec4(0, 1, 0, 0) * glm::rotate(glm::mat4(1.f), rocket->getXRotation(), glm::vec3(1, 0, 0));

}

/*
* Create View Matrix
*/
glm::mat4 Camera::getCameraViewMatrix()
{
    glm::vec3 rocketPos = rocket->getPosition();
    glm::vec3 rocketFwd = rocket->getForward();
    return glm::lookAt(pos, rocketPos, upVector);
}
glm::mat4 Camera::getCameraProjectionMatrix()
{
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    float aspect = width / (float)height;
    glm::mat4 p = glm::perspective(glm::radians(50.0f), aspect, 0.1f, 200.0f);
    return p;
}

/*
* Matrix used for skybox. Does not take into account position, only rotation
*/
glm::mat4 Camera::getCameraRotationMatrix()
{
    return glm::lookAt(glm::vec3(0), rocket->getPosition() - pos, upVector);
}
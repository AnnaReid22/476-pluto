#include "Camera.h"
#include "Time.h"
#include "GameObject.h"

#include <iostream>

Camera::Camera(GameObject* d_GameObject): Component(d_GameObject)
{
    // Distance between camera and rocket
    camDist = 3.0f;

    upVector = glm::vec3(0, 1, 0);

    // Current camera position
    pos = glm::vec3(0.f);
}

/*
* Updates camera position based on spring effect
*/
void Camera::setUpCam()
{
    float f = 0.01;
    pos = pos + ((rocket->getPosition() + rocket->getForward() * camDist) - pos) * f;
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
    glm::vec3 rocketPos = rocket->getPosition();
    glm::vec3 rocketFwd = rocket->getForward();
    return glm::lookAt(pos, rocketPos, upVector);
}

/*
* Matrix used for skybox. Does not take into account position, only rotation
*/
glm::mat4 Camera::getCameraRotationMatrix()
{
    return glm::lookAt(glm::vec3(0), rocket->getPosition() - pos, upVector);
}
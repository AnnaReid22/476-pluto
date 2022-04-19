#include "Camera.h"
#include "Time.h"
#include "GameObject.h"

Camera::Camera(GameObject* d_GameObject) : Component(d_GameObject)
{
    phi = 0.0;
    theta = 3 * 3.14159 / 2;
    eyeOffset = glm::vec3(0, 0, 0);
    lookAt = glm::vec3(0, 0, -0.5);
    gaze = normalize(lookAt - eyeOffset);
    upVector = glm::vec3(0, 1, 0);
    lastX = 0;
    lastY = 0;
    firstMouse = true;
    speed = 10;
    sensitivity = 0.2;

    dollyB = false;
    dollyF = false;
    strafeR = false;
    strafeL = false;
    rise = false;
    fall = false;

    windowManager = nullptr;
}

void Camera::setUpCam(WindowManager* wm)
{
    // moveLookAt(wm);
    lookAt = this->gameObject->transform.position;
    moveEye();
}

void Camera::setEyePos(glm::vec3 pos)
{
    eyeOffset = pos;
}

void Camera::setSpeed(float s)
{
    speed = s;
}

void Camera::setSens(float sns)
{
    sensitivity = sns;
}

void Camera::enableCursor(WindowManager* windowManager)
{
    glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPos(windowManager->getHandle(), lastX, lastY);
}

void Camera::Update()
{
    setUpCam(windowManager);
}

glm::mat4 Camera::getCameraViewMatrix()
{
    return glm::lookAt(this->gameObject->transform.position + eyeOffset, lookAt, upVector);
}

void Camera::moveLookAt(WindowManager* windowManager)
{
    double xpos, ypos;
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    glfwGetCursorPos(windowManager->getHandle(), &xpos, &ypos);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double dx = xpos - lastX;
    double dy = ypos - lastY;

    phi -= ((dy / height) * 2 * 3.14159) * sensitivity;
    if (phi > glm::radians(80.0))
        phi = glm::radians(80.0);
    if (phi < -glm::radians(80.0))
        phi = -glm::radians(80.0);

    theta += ((dx / width) * (2 * 3.14159)) * sensitivity;

    lastX = xpos;
    lastY = ypos;

    lookAt = glm::vec3(
        (this->gameObject->transform.position.x + eyeOffset.x) + 10 * glm::cos(phi) * glm::cos(theta),
        (this->gameObject->transform.position.y + eyeOffset.y) + 10 * glm::sin(phi),
        (this->gameObject->transform.position.z + eyeOffset.z) + 10 * glm::cos(phi) * glm::cos((3.14159 / 2.0) - theta
        ));
}

void Camera::moveEye()
{
    Time* time = time->getInstance();
    float frametime = time->getFrametime();

    gaze = normalize(lookAt - (this->gameObject->transform.position + eyeOffset));
    glm::vec3 strafeAxis = normalize(cross(gaze, upVector));

    if (dollyF)
    {
        this->gameObject->transform.position += speed * normalize(glm::vec3(gaze.x, 0, gaze.z)) * frametime;
    }
    if (dollyB)
    {
        this->gameObject->transform.position -= speed * normalize(glm::vec3(gaze.x, 0, gaze.z)) * frametime;
    }
    if (strafeR)
    {
        this->gameObject->transform.position += speed * strafeAxis * frametime;
    }
    if (strafeL)
    {
        this->gameObject->transform.position -= speed * strafeAxis * frametime;
    }
    if (rise)
    {
        this->gameObject->transform.position += speed * upVector * frametime;
    }
    if (fall)
    {
        this->gameObject->transform.position -= speed * upVector * frametime;
    }

}
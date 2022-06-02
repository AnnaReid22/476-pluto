#include "Transform.h"
#include <glm/gtx/quaternion.hpp>

Transform::Transform()
{
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
    rotation = glm::quat(1.0, 0.0, 0.0, 0.0);
    hierarchicalRot = glm::quat(1.0, 0.0, 0.0, 0.0);
    hierarchicalTrans = glm::vec3(0.0f, 0.0f, 0.0f);
}

Transform::Transform(glm::vec3 d_Pos, glm::vec3 d_Scale)
{
    position = d_Pos;
    scale = d_Scale;
    rotation = glm::quat(1.0, 0.0, 0.0, 0.0);
    hierarchicalRot = glm::quat(1.0, 0.0, 0.0, 0.0);
}

Transform::Transform(glm::vec3 d_Pos, glm::vec3 d_Scale, glm::quat d_Rot)
{
    position = d_Pos;
    scale = d_Scale;
    rotation = d_Rot;
    hierarchicalRot = glm::quat(1.0, 0.0, 0.0, 0.0);
}

glm::mat4 Transform::genModelMatrix()
{
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), this->scale);
    glm::mat4 rotMatrix = glm::toMat4(this->rotation);
    glm::mat4 transMatrix = glm::translate(glm::mat4(1.0f), this->position);

    return transMatrix * rotMatrix * scaleMatrix;
}

glm::mat4 Transform::getRotMatrix()
{
    return glm::toMat4(this->rotation);
}
glm::mat4 Transform::getScaleMatrix()
{
    return glm::scale(glm::mat4(1.0f), this->scale);
}
glm::mat4 Transform::getTransMatrix()
{
    return glm::translate(glm::mat4(1.0f), this->position);
}
glm::mat4 Transform::getHierarchicalRot()
{
    return glm::toMat4(this->hierarchicalRot);
}

glm::mat4 Transform::getHierarchicalTrans()
{
    return glm::translate(glm::mat4(1.0f), this->hierarchicalTrans);
}

glm::mat4 Transform::genHierarchicalMatrix()
{
    return getHierarchicalTrans() * getHierarchicalRot() * getTransMatrix() * getRotMatrix() * getScaleMatrix();
}

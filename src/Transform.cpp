#include "Transform.h"
#include <glm/gtx/quaternion.hpp>

Transform::Transform()
{
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
    rotation = glm::quat(1.0, 0.0, 0.0, 0.0);
    hierarchicalRot1 = glm::quat(1.0, 0.0, 0.0, 0.0);
    hierarchicalTrans2 = glm::vec3(0.0f, 0.0f, 0.0f);
    hierarchicalRot2 = glm::quat(1.0, 0.0, 0.0, 0.0);
    hierarchicalTrans1 = glm::vec3(0.0f, 0.0f, 0.0f);
}

Transform::Transform(glm::vec3 d_Pos, glm::vec3 d_Scale)
{
    position = d_Pos;
    scale = d_Scale;
    rotation = glm::quat(1.0, 0.0, 0.0, 0.0);
    hierarchicalRot1 = glm::quat(1.0, 0.0, 0.0, 0.0);
}

Transform::Transform(glm::vec3 d_Pos, glm::vec3 d_Scale, glm::quat d_Rot)
{
    position = d_Pos;
    scale = d_Scale;
    rotation = d_Rot;
    hierarchicalRot1 = glm::quat(1.0, 0.0, 0.0, 0.0);
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
glm::mat4 Transform::getHierarchicalRot1Matrix()
{
    return glm::toMat4(this->hierarchicalRot1);
}

glm::mat4 Transform::getHierarchicalTrans2Matrix()
{
    return glm::translate(glm::mat4(1.0f), this->hierarchicalTrans2);
}

glm::mat4 Transform::genHierarchicalMatrix()
{
    return getHierarchicalTrans2Matrix() * getHierarchicalRot1Matrix() * getTransMatrix() * getHierarchicalRot2Matrix() * getHierarchicalTrans1Matrix() * getRotMatrix() * getScaleMatrix();
}

glm::mat4 Transform::getUndoTransMatrix()
{
    return glm::translate(glm::mat4(1.0f), -1.0f*(this->position));
}
glm::mat4 Transform::getHierarchicalTrans1Matrix()
{
    return glm::translate(glm::mat4(1.0f), this->hierarchicalTrans1);
}
glm::mat4 Transform::getUndoHierarchicalTrans1Matrix()
{
    return glm::translate(glm::mat4(1.0f), -1.0f*this->hierarchicalTrans1);
}
glm::mat4 Transform::getHierarchicalRot2Matrix()
{
    return glm::toMat4(this->hierarchicalRot2);
}

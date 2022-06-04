#pragma once
#define _USE_MATH_DEFINES
#include <glm/gtc/type_ptr.hpp>
#include <cmath>     //for M_PI

class Transform
{
public:
    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;
    glm::quat hierarchicalRot1;
    glm::vec3 hierarchicalTrans2;
    //Used for fin rotations
    glm::vec3 hierarchicalTrans1;
    glm::quat hierarchicalRot2;
    //Transform* parentTransform

    Transform();
    Transform(glm::vec3 d_Pos, glm::vec3 d_Scale);
    Transform(glm::vec3 d_Pos, glm::vec3 d_Scale, glm::quat d_Rot);


    glm::mat4 genModelMatrix();
    glm::mat4 getRotMatrix();
    glm::mat4 getScaleMatrix();
    glm::mat4 getTransMatrix();
    glm::mat4 getHierarchicalRot1Matrix();
    glm::mat4 getHierarchicalTrans2Matrix();
    glm::mat4 genHierarchicalMatrix();

    //Used for fin rotations
    glm::mat4 getUndoTransMatrix();
    glm::mat4 getHierarchicalTrans1Matrix();
    glm::mat4 getUndoHierarchicalTrans1Matrix();
    glm::mat4 getHierarchicalRot2Matrix();

};


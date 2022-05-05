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

    Transform();
    Transform(glm::vec3 d_Pos, glm::vec3 d_Scale);
    Transform(glm::vec3 d_Pos, glm::vec3 d_Scale, glm::quat d_Rot);

    glm::mat4 genModelMatrix();
};


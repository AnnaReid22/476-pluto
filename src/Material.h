#pragma once

#include <memory>

#include "Texture.h"
#include <glm/gtc/type_ptr.hpp>

class Material
{
public:

    std::shared_ptr<Texture> t_albedo;
    std::shared_ptr<Texture> t_normal;

    glm::vec3 m_diffuse;
    glm::vec3 m_spec;
    glm::vec3 m_shine;
    glm::vec3 m_ambient;
};


#include "ResourceManager.h"

#include <iostream>

GLuint ResourceManager::getRenderTextureResource(std::string name)
{
    return getGeneric<GLuint>(name, renderTextures);
}

float ResourceManager::getNumericalValue(std::string name)
{
    return getGeneric<float>(name, numericalValues);
}

std::shared_ptr<Shape> ResourceManager::getMesh(std::string name)
{
    return getGeneric<std::shared_ptr<Shape>>(name, meshes);
}

std::shared_ptr<Texture> ResourceManager::getUserTextureResource(std::string name)
{
    return getGeneric<std::shared_ptr<Texture>>(name, userTextures);
}

glm::vec2 ResourceManager::getVec2(std::string name)
{
    return getGeneric<glm::vec2>(name, vector2Ds);
}

glm::vec3 ResourceManager::getVec3(std::string name)
{
    return getGeneric<glm::vec3>(name, vector3Ds);
}

glm::vec4 ResourceManager::getVec4(std::string name)
{
    return getGeneric<glm::vec4>(name, vector4Ds);
}

void* ResourceManager::getOther(std::string name)
{
    return getGeneric<void*>(name, others);
}

void ResourceManager::addRenderTextureResource(std::string name, GLuint item)
{
    renderTextures[name] = item;
}

void ResourceManager::addNumericalValue(std::string name, float item)
{
    numericalValues[name] = item;
}

void ResourceManager::addMesh(std::string name, std::shared_ptr<Shape> item)
{
    meshes[name] = item;
}

void ResourceManager::addUserTextureResource(std::string name, std::shared_ptr<Texture> item)
{
    userTextures[name] = item;
}

void ResourceManager::addVec2(std::string name, glm::vec2 item)
{
    vector2Ds[name] = item;
}

void ResourceManager::addVec3(std::string name, glm::vec3 item)
{
    vector3Ds[name] = item;
}

void ResourceManager::addVec4(std::string name, glm::vec4 item)
{
    vector4Ds[name] = item;
}

void ResourceManager::addOther(std::string name, void* item)
{
    others[name] = item;
}

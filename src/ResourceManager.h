#pragma once

#include <unordered_map>
#include <string>

#include <glad/glad.h>
#include <memory>
#include "Shape.h"
#include "Texture.h"


class ResourceManager
{
private:
    static ResourceManager* instance;

    std::unordered_map<std::string, GLuint> renderTextures;
    std::unordered_map<std::string, float> numericalValues;
    std::unordered_map<std::string, std::shared_ptr<Shape>> meshes;
    std::unordered_map<std::string, std::shared_ptr<Texture>> userTextures;

    std::unordered_map<std::string, glm::vec3> vector3Ds;
    std::unordered_map<std::string, glm::vec2> vector2Ds;
    std::unordered_map<std::string, glm::vec4> vector4Ds;

    std::unordered_map<std::string, void*> others;

    template<typename T>
    T getGeneric(std::string name, std::unordered_map<std::string, T> mapToSearch);

public:
    static ResourceManager* getInstance() {
        if (!instance)
            instance = new ResourceManager;
        return instance;
    }

    GLuint getRenderTextureResource(std::string name);
    float getNumericalValue(std::string name);
    std::shared_ptr<Shape> getMesh(std::string name);
    std::shared_ptr<Texture> getUserTextureResource(std::string name);
    glm::vec2 getVec2(std::string name);
    glm::vec3 getVec3(std::string name);
    glm::vec4 getVec4(std::string name);
    void* getOther(std::string name);

    void addRenderTextureResource(std::string name, GLuint item);
    void addNumericalValue(std::string name, float item);
    void addMesh(std::string name, std::shared_ptr<Shape> item);
    void addUserTextureResource(std::string name, std::shared_ptr<Texture> item);
    void addVec2(std::string name, glm::vec2 item);
    void addVec3(std::string name, glm::vec3 item);
    void addVec4(std::string name, glm::vec4 item);
    void addOther(std::string name, void* item);

};

template<typename T>
inline T ResourceManager::getGeneric(std::string name, std::unordered_map<std::string, T> mapToSearch)
{
    try {
        return mapToSearch.at(name);
    }
    catch (int num)
    {
        std::cout << "Requested resource: " << name << " is not known by the Resource Manager." << std::endl;
        exit(num);
    }
}

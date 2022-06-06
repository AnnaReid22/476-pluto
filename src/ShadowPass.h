#pragma once
#include "IShader.h"
#include "ResourceManager.h"
#include "Material.h"


#include <memory>

class ShadowPass :
    public IShader
{
private:
    ResourceManager* rm;

    std::shared_ptr<Program> depthProg;
    bool SHADOW = true;
    GLuint depthMapFBO;
    GLuint depthMap;
    glm::vec3 g_light = glm::vec3(0.0, 5.0, 7.0);
    GLuint quad_VertexArrayID;
    GLuint quad_vbuf;

public:
    ShadowPass() {
        rm = rm->getInstance();
    };

    void init();
    void execute(WindowManager * windowManager);
    void draw(std::shared_ptr<Program> prog) ;
};


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
    std::shared_ptr<Program> shadowProg;
    std::shared_ptr<Program> depthProgDebug;
    std::shared_ptr<Program> shadowDebugProg;
    bool DEBUG_LIGHT = false;
	bool GEOM_DEBUG = false;
    bool SHADOW = true;
    GLuint depthMapFBO;
    const GLuint S_WIDTH = 1024, S_HEIGHT = 1024;
    GLuint depthMap;
    vec3 g_light = vec3(3, 5, 5);

public:
    ShadowPass() {
        rm = rm->getInstance();
    };

    void init();
    void execute(WindowManager * windowManager);
    void draw(std::shared_ptr<Program> prog, GLint texID) ;
};


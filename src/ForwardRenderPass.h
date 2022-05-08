#pragma once
#include "IShader.h"

#include "ResourceManager.h"
#include "Material.h"

class ForwardRenderPass :
    public IShader
{
private:
    ResourceManager* rm;

    std::shared_ptr<Program> prog;
    std::shared_ptr<Program> skyProg;

public:
    ForwardRenderPass() {
        rm = rm->getInstance();
    };

    std::shared_ptr<Texture> skyboxTexture;
    std::shared_ptr<Texture> skyboxNoiseTexture;
    
    std::shared_ptr<Shape> skyboxMesh;

    void init();
    void execute(WindowManager * windowManager);
};


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
public:
    ForwardRenderPass() {
        rm = rm->getInstance();
    };

    std::shared_ptr<Material> skyboxMaterial;
    std::shared_ptr<Shape> skyboxMesh;

    void init();
    void execute(WindowManager * windowManager);
};


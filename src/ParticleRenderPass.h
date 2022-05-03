#pragma once
#include "IShader.h"

#include "ResourceManager.h"
#include "Material.h"

class ParticleRenderPass :
    public IShader
{
private:
    ResourceManager* rm;

    std::shared_ptr<Program> particleProg;
public:
    ParticleRenderPass() {
        rm = rm->getInstance();
    };

    Texture* particleTexture;

    void init();
    void execute(WindowManager * windowManager);
};


#pragma once
#include "IShader.h"

#include "ResourceManager.h"
#include "Material.h"

#include <memory>

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

    std::shared_ptr<Texture> particleTexture;

    void init();
    void execute(WindowManager * windowManager);
};

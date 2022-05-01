#pragma once

#include <vector>
#include <memory>
#include <glm/gtc/type_ptr.hpp>

#include "GameObject.h"
#include "Camera.h"
#include "Program.h"
#include "MeshRenderer.h"
#include "IShader.h"
#include "ResourceManager.h"

class RenderPipeline
{
private:
    WindowManager* windowManager;
    ResourceManager* rm;

    std::vector<std::shared_ptr<IShader>> renderPasses;

    std::shared_ptr<IShader> outputPass;

    std::vector<GameObject*> viewFrustumCull(std::vector<GameObject*> objectsToRender, Camera* cam);
public:

    RenderPipeline() = default;
    RenderPipeline(WindowManager* wm);

    void addRenderPass(std::shared_ptr<IShader> pass);

    void executePipeline();
};


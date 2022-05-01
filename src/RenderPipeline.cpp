#include "RenderPipeline.h"
#include "MeshRenderer.h"
#include "GameObject.h"

RenderPipeline::RenderPipeline(WindowManager* wm)
{
    windowManager = wm;
    rm = rm->getInstance();
}

std::vector<GameObject *> RenderPipeline::viewFrustumCull(std::vector<GameObject*> objectsToRender, Camera* cam)
{
    // TODO: weeeee view frustum culling

    return objectsToRender;
}

void RenderPipeline::addRenderPass(std::shared_ptr<IShader> pass)
{
    renderPasses.push_back(pass);
    pass->init();
}

void RenderPipeline::executePipeline()
{
    Camera* cam = (Camera*)rm->getOther("activeCamera");
    std::vector<GameObject*> renderables = *(std::vector<GameObject*> *)rm->getOther("renderables");
    renderables = viewFrustumCull(renderables, cam);

    rm->addOther("renderables", &renderables);

    for (std::shared_ptr<IShader> pass : renderPasses)
        pass->execute(windowManager);

    //outputPass->execute(windowManager);

    return;
}



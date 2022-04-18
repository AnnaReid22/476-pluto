#pragma once

#include <vector>
#include <memory>
#include <glm/gtc/type_ptr.hpp>

#include "GameObject.h"
#include "Camera.h"
#include "Program.h"

class RenderPipeline
{
private:
    WindowManager* windowManager;

    glm::mat4 GetProjectionMatrix(); 
public:
    std::shared_ptr<Program> prog;

    RenderPipeline() = default;
    RenderPipeline(WindowManager* wm);

    void renderFrame(std::vector<GameObject*> objectsToRender, Camera* cam);
};


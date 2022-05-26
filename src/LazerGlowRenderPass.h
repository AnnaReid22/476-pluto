#pragma once
#include "IShader.h"
#include "Program.h"

#include <memory>

class LazerGlowRenderPass :
    public IShader
{
private:

    GLuint colorBuffer, frameBufferObject;

    std::shared_ptr<Program> prog;

public:
    void init();
    void execute(WindowManager* windowManager);
};


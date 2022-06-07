#pragma once
#include "IShader.h"
#include <memory>
#include "Program.h"

#include "ResourceManager.h"
class DeferredSamplingPass :
    public IShader
{
private:
    GLuint deferredFBO;
    GLuint depthRenderBuffer;
    GLuint gBuffer, gNormal, gColor;

    std::shared_ptr<Program> prog, rocket_prog;
    ResourceManager* rm = nullptr;

public:
    void init();
    void execute(WindowManager* windowManager);

};


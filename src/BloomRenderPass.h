#pragma once
#include "IShader.h"

#include <memory>
#include "ResourceManager.h"
#include "Program.h"
class BloomRenderPass :
    public IShader
{
private:
    ResourceManager* rm;

    std::shared_ptr<Program> filterBrightProg;
    std::shared_ptr<Program> blurProg;

    GLuint filterBrightFB;
    GLuint blurFB[2];

    GLuint blurPassTexture[2];

    GLuint quad_VertexArrayID, quad_vertexbuffer;

    void initQuad();
    void execute_blur_pass(bool horizontal);

public:
    void init();
    void execute(WindowManager* windowManager);
};


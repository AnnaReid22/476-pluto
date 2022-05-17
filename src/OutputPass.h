#pragma once
#include "IShader.h"
#include <memory>
#include "Program.h"
#include "ResourceManager.h"

class OutputPass :
    public IShader
{
private:
    std::shared_ptr<Program> prog;
    void initQuad();

    GLuint quad_VertexArrayID, quad_vertexbuffer;
    ResourceManager* rm = nullptr;


public:
    void init();
    void execute(WindowManager* windowManager);
};


#pragma once
#include "IShader.h"
#include "Program.h"

#include <memory>

class GuiRenderPass :
    public IShader
{
private:

    int numWhitePoints;
    int numOrangePoints;

    GLuint colorBuffer, frameBufferObject;

    GLuint rectBufId;
    GLuint vaoID;

    std::shared_ptr<Program> prog;

    void setGeometry(int lives, int points, float distToGoal);
public:
    void init();
    void execute(WindowManager* windowManager);
};

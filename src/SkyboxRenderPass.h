#pragma once
#include "IShader.h"
#include "Shape.h"
#include "Texture.h"
#include "Program.h"

#include <memory>

class SkyboxRenderPass :
    public IShader
{
private:

    GLuint colorBuffer, frameBufferObject;

    std::shared_ptr<Shape> texSphere;
    std::shared_ptr<Texture> sky_noise_albedo;
    std::shared_ptr<Texture> sky_texture_albedo;
    std::shared_ptr<Program> prog;

public:
    void init();
    void execute(WindowManager *windowManager);
};


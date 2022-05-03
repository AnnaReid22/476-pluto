#include "ParticleRenderPass.h"
#include "ParticleSystem.h"
#include "Program.h"
#include "GameObject.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"

glm::mat4 GetTheProjectionMatrix(WindowManager * windowManager)
{
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    float aspect = width / (float)height;
    glm::mat4 Projection = glm::perspective(glm::radians(50.0f), aspect, 0.1f, 200.0f);
    return Projection;
}

void ParticleRenderPass::init()
{
    particleProg = std::make_shared<Program>();
    particleProg->setVerbose(true);
    particleProg->setShaderNames("../shaders/ps_vert.glsl", "../shaders/ps_frag.glsl");
    if (! particleProg->init())
    {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    particleProg->addUniform("P");
    particleProg->addUniform("M");
    particleProg->addUniform("V");
    particleProg->addUniform("alphaTexture");
    particleProg->addAttribute("colorPos");
    particleProg->addAttribute("vertPos");
}

void ParticleRenderPass::execute(WindowManager * windowManager)
{
    // initialize the particle system correctly
    // get all transformation matrices correctly
    Camera* cam = (Camera*)rm->getOther("activeCamera");

    glm::mat4 P = GetTheProjectionMatrix(windowManager);
    glm::mat4 V = cam->getCameraRotationMatrix();
    glm::mat4 M = glm::mat4(1);
    
    particleProg->bind();
    this->particleTexture->bind(particleProg->getUniform("alphaTexture"));
    glUniformMatrix4fv(particleProg->getUniform("P"), 1, GL_FALSE, value_ptr(P));
    glUniformMatrix4fv(particleProg->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    glUniformMatrix4fv(particleProg->getUniform("M"), 1, GL_FALSE, value_ptr(M));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(15.0f);
    // partSystem->drawMe(particleProg);
    // partSystem->update();
    // thePartSystem2->drawMe(particleProg);
    // thePartSystem2->update();
    glBlendFunc(GL_ONE, GL_ZERO);

    particleProg->unbind();
    assert(glGetError() == GL_NO_ERROR);
}

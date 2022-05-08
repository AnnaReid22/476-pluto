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
    //initializing the particle system
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
    // get all transformation matrices correctly
    Camera* cam = (Camera*)rm->getOther("activeCamera");
    std::vector<GameObject*> partSystems = *(std::vector<GameObject*> *)rm->getOther("particleSystem");

    glm::mat4 P = GetTheProjectionMatrix(windowManager);
    glm::mat4 V = cam->getCameraViewMatrix();

    particleTexture = rm->getUserTextureResource("particleTexture");
    
    particleProg->bind();
    this->particleTexture->bind(particleProg->getUniform("alphaTexture"));
    glUniformMatrix4fv(particleProg->getUniform("P"), 1, GL_FALSE, value_ptr(P));
    glUniformMatrix4fv(particleProg->getUniform("V"), 1, GL_FALSE, value_ptr(V));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(11.0f);

    for (GameObject* obj : partSystems)
    {
        Player* pl = ((GameObject*)rm->getOther("player_game_object"))->getComponentByType<Player>();
        glm::vec3 fwd = pl->getForward();
        glm::mat4 fwdMatrix = glm::translate(glm::mat4(1.0f), fwd);
        glm::mat4  M = obj->transform.genModelMatrix() + fwdMatrix;
        ParticleSystem* ps = obj->getComponentByType<ParticleSystem>();
        glUniformMatrix4fv(particleProg->getUniform("M"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
        ps->draw(particleProg);
    }

    glBlendFunc(GL_ONE, GL_ZERO);

    particleProg->unbind();
    assert(glGetError() == GL_NO_ERROR);
}

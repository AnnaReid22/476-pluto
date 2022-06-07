#include "ParticleRenderPass.h"
#include "ParticleSystem.h"
#include "Program.h"
#include "GameObject.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"
#include "Player.h"

void ParticleRenderPass::init()
{

    this->rm = rm->getInstance();
	float width = rm->getNumericalValue("screenWidth");
	float height = rm->getNumericalValue("screenHeight");

	glGenFramebuffers(1, &particleSysFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, particleSysFBO);

	// - position buffer
	glGenTextures(1, &psPositionOutput);
	glBindTexture(GL_TEXTURE_2D, psPositionOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, psPositionOutput, 0);

    rm->addRenderTextureResource("psPositionOutput", psPositionOutput);

	// - color buffer
	glGenTextures(1, &psColorOutput);
	glBindTexture(GL_TEXTURE_2D, psColorOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, psColorOutput, 0);

    rm->addRenderTextureResource("psColorOutput", psColorOutput);

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
    particleProg->addAttribute("psColor");
    particleProg->addAttribute("psPosition");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ParticleRenderPass::execute(WindowManager * windowManager)
{
    Camera* cam = (Camera*)rm->getOther("activeCamera");
    std::vector<GameObject*> partSystems = *(std::vector<GameObject*> *)rm->getOther("particleSystem");

    glm::mat4 P = cam->getCameraProjectionMatrix();
    glm::mat4 V = cam->getCameraViewMatrix();

    particleTexture = rm->getUserTextureResource("particleTexture");

    //particle system with deferred rendering
    glBindFramebuffer(GL_FRAMEBUFFER, particleSysFBO);

	GLenum buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffers);

	int width, height;

	glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
	float aspect = width / (float)height;
	glViewport(0, 0, width, height);
	// Clear framebuffer.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	particleProg->bind();

        this->particleTexture->bind(particleProg->getUniform("alphaTexture"));
        glUniformMatrix4fv(particleProg->getUniform("P"), 1, GL_FALSE, value_ptr(P));
        glUniformMatrix4fv(particleProg->getUniform("V"), 1, GL_FALSE, value_ptr(V));

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPointSize(50.0f);

        for (GameObject* obj : partSystems)
        {
            Player* pl = ((GameObject*)rm->getOther("player_game_object"))->getComponentByType<Player>();
            ParticleSystem* ps = obj->getComponentByType<ParticleSystem>();
            glUniformMatrix4fv(particleProg->getUniform("M"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
            if(obj->isEnabled)
            {
                ps->draw(particleProg);
            }
        }
        glBlendFunc(GL_ONE, GL_ZERO);
	particleProg->unbind();

    glDrawBuffers(1, buffers);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, psPositionOutput);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, psColorOutput);
    glGenerateMipmap(GL_TEXTURE_2D);

    //vertex = M * V * position
    // assert(glGetError() == GL_NO_ERROR);
}

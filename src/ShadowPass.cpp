#include "ParticleRenderPass.h"
#include "ParticleSystem.h"
#include "Program.h"
#include "GameObject.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"
#include "ShadowPass.h"

void ShadowPass::init() 
{
	this->rm = rm->getInstance();
	float width = rm->getNumericalValue("screenWidth");
	std::cout << width << std::endl;
	float height = rm->getNumericalValue("screenHeight");
	std::cout << height << std::endl;
	// Set background color.
  	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	 // Enable z-buffer test.
  	glEnable(GL_DEPTH_TEST);
    // Initialize programs
  	depthProg = std::make_shared<Program>();
  	depthProg->setVerbose(true);
  	depthProg->setShaderNames("../shaders/depth_vert.glsl", "../shaders/depth_frag.glsl");
  	depthProg->init();

    // Add uniform and attributes
  	depthProg->addUniform("LP");
  	depthProg->addUniform("LV");
  	depthProg->addUniform("M");
  	depthProg->addAttribute("vertPos");
  	depthProg->addAttribute("vertNor");
  	depthProg->addAttribute("vertTex");

    //generate the FBO for the shadow depth
  	glGenFramebuffers(1, &depthMapFBO);

    //generate the texture
  	glGenTextures(1, &depthMap);
  	glBindTexture(GL_TEXTURE_2D, depthMap);
  	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 8192.0f, 8192.0f, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //bind with framebuffer's depth buffer
  	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  	glDrawBuffer(GL_NONE);
  	glReadBuffer(GL_NONE);
  	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	rm->addRenderTextureResource("shadowOutput", depthMap);
}

void ShadowPass::execute(WindowManager * windowManager)
{
	float width = rm->getNumericalValue("screenWidth");
	float height = rm->getNumericalValue("screenHeight");

	glm::vec3 fwd = ((GameObject*)rm->getOther("player_game_object"))->getComponentByType<Player>()->getForward();
	
    vec3 lightLA = vec3(0.0, 0.0, -1000.0);
    vec3 lightUp = vec3(0, 1, 0);

    mat4 LO, LV;

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	depthProg->bind();

		//light orthogonal view
		LO = glm::ortho(-300.0f, 300.0f, -300.0f, 300.0f, 10.0f, 1000.0f);  
		glUniformMatrix4fv(depthProg->getUniform("LP"), 1, GL_FALSE, glm::value_ptr(LO));

		//light view
		LV = glm::lookAt(g_light, lightLA, lightUp);
		glUniformMatrix4fv(depthProg->getUniform("LV"), 1, GL_FALSE, glm::value_ptr(LV));
		draw(depthProg);

	depthProg->unbind();
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glGenerateMipmap(GL_TEXTURE_2D);

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ShadowPass::draw(std::shared_ptr<Program> prog) 
{
    std::vector<GameObject*> renderables = *(std::vector<GameObject*> *)rm->getOther("lightingRenderables");
    mat4 M;
    for (GameObject* obj : renderables)
    {
        M = obj->transform.genModelMatrix();
        MeshRenderer* mr = obj->getComponentByType<MeshRenderer>();
        std::shared_ptr<Shape> mesh = mr->mesh;
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, glm::value_ptr(M));
        mesh->draw(prog);
    }
}

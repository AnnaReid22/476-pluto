#include "ForwardRenderPass.h"

#include "Program.h"
#include "GameObject.h"
#include "Camera.h"
#include "MeshRenderer.h"

#include "ResourceManager.h"
#include "Transform.h"

glm::mat4 GetProjectionMatrix(WindowManager * windowManager)
{
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    float aspect = width / (float)height;
    glm::mat4 Projection = glm::perspective(glm::radians(50.0f), aspect, 0.1f, 200.0f);
    return Projection;
}

void ForwardRenderPass::init()
{
    std::string resourceDirectory = "../resources";

	prog = std::make_shared<Program>();
	prog->setVerbose(true);
	prog->setShaderNames("../shaders/tex_vert.glsl", "../shaders/tex_frag0.glsl");
	prog->init();
	prog->addUniform("P");
	prog->addUniform("V");
	prog->addUniform("M");
	prog->addUniform("Texture0");
	prog->addAttribute("vertPos");
	prog->addAttribute("vertNor");
	prog->addAttribute("vertTex");
	prog->addUniform("lightPos");
	prog->addUniform("MatShine");
	prog->addUniform("flip");
	prog->addAttribute("vertNor");

  skyProg = std::make_shared<Program>();
  skyProg->setVerbose(true);
  skyProg->setShaderNames("../shaders/sky_vert.glsl", "../shaders/sky_frag.glsl");
  skyProg->init();
  skyProg->addUniform("SkyMat");
  skyProg->addUniform("time");
  skyProg->addUniform("Texture0");
  skyProg->addUniform("TwinkleNoise");
  skyProg->addAttribute("vertPos");
  skyProg->addAttribute("vertNor");
  skyProg->addAttribute("vertTex");
}

void ForwardRenderPass::execute(WindowManager * windowManager)
{
    skyboxMesh = rm->getMesh("skybox");
    skyboxTexture = rm->getUserTextureResource("skybox");
    skyboxNoiseTexture = rm->getUserTextureResource("skyboxNoise");

    // Get current frame buffer size.
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    glViewport(0, 0, width, height);

    // Clear framebuffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = width / (float)height;

    Camera* cam = (Camera*)rm->getOther("activeCamera");
    std::vector<GameObject*> renderables = *(std::vector<GameObject*> *)rm->getOther("renderables");

    skyProg->bind();

    glm::mat4 SkyMat = cam->getCameraProjectionMatrix() * cam->getCameraRotationMatrix();

    glUniformMatrix4fv(skyProg->getUniform("SkyMat"), 1, GL_FALSE, glm::value_ptr(SkyMat));
    glUniform1f(skyProg->getUniform("time"), Time::getInstance()->getGlobalTime());

    glDisable(GL_DEPTH_TEST);

    this->skyboxTexture->bind(skyProg->getUniform("Texture0")); 
    this->skyboxNoiseTexture->bind(skyProg->getUniform("TwinkleNoise"));
    this->skyboxMesh->draw(skyProg);
    this->skyboxNoiseTexture->unbind();
    this->skyboxTexture->unbind();

    glEnable(GL_DEPTH_TEST);

    skyProg->unbind();
    prog->bind();

    glm::mat4 P = cam->getCameraProjectionMatrix();
    glm::mat4 V = cam->getCameraViewMatrix();
    glm::mat4 M = glm::mat4(1);

    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, glm::value_ptr(V));
    glUniform3f(prog->getUniform("lightPos"), 0.0, 2.0, 0.0);
    glUniform1f(prog->getUniform("MatShine"), 23);
    glUniform1i(prog->getUniform("flip"), 1);
    for (GameObject* obj : renderables)
    {
        //M = obj->transform.genModelMatrix();
        glm::mat4 Mchild = obj->transform.genModelMatrix();
        if (obj->parent)
        {
            //glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), 4.0f);

            M = obj->parentObj->transform.genModelMatrix() *obj->transform.getHierarchicalRot() * obj->transform.getTransMatrix() * obj->transform.getRotMatrix() * obj->transform.getScaleMatrix();
            //std::cout << "compare:" << obj << std::endl;
            //std::cout << "player compare: " << obj->parentObj << std::endl;;
        }
        else
        {
            M = Mchild;
        }
        MeshRenderer* mr = obj->getComponentByType<MeshRenderer>();

        std::shared_ptr<Material> mat = mr->material;
        std::shared_ptr<Shape> mesh = mr->mesh;

        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, glm::value_ptr(M));

        mat->t_albedo->bind(prog->getUniform("Texture0"));

        mesh->draw(prog);

        mat->t_albedo->unbind();
    }

    prog->unbind();
    assert(glGetError() == GL_NO_ERROR);
}

#include "ForwardRenderPass.h"

#include "Program.h"
#include "GameObject.h"
#include "Camera.h"
#include "MeshRenderer.h"

#include "ResourceManager.h"

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
}

void ForwardRenderPass::execute(WindowManager * windowManager)
{
    skyboxMesh = rm->getMesh("skybox");
    skyboxMaterial = *((std::shared_ptr<Material> *)rm->getOther("skyboxMat"));

    // Get current frame buffer size.
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    glViewport(0, 0, width, height);

    // Clear framebuffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = width / (float)height;

    Camera* cam = (Camera*)rm->getOther("activeCamera");
    std::vector<GameObject*> renderables = *(std::vector<GameObject*> *)rm->getOther("renderables");

    prog->bind();

    glm::mat4 P = GetProjectionMatrix(windowManager);
    glm::mat4 V = cam->getCameraRotationMatrix();
    glm::mat4 M = glm::mat4(1);

    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, glm::value_ptr(V));
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, glm::value_ptr(M));
    glUniform3f(prog->getUniform("lightPos"), 0.0, 10.0, 0.0);
    glUniform1f(prog->getUniform("MatShine"), 1000);
    glUniform1i(prog->getUniform("flip"), 1);

    glDisable(GL_DEPTH_TEST);

    this->skyboxMaterial->t_albedo->bind(prog->getUniform("Texture0"));
    this->skyboxMesh->draw(prog);
    this->skyboxMaterial->t_albedo->unbind();

    glEnable(GL_DEPTH_TEST);

    V = cam->getCameraViewMatrix();
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, glm::value_ptr(V));
    glUniform3f(prog->getUniform("lightPos"), 0.0, 2.0, 0.0);
    glUniform1f(prog->getUniform("MatShine"), 23);
    glUniform1i(prog->getUniform("flip"), 1);
    for (GameObject* obj : renderables)
    {
        M = obj->transform.genModelMatrix();
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

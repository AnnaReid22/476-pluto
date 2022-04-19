#include "RenderPipeline.h"
#include "MeshRenderer.h"
#include "GameObject.h"

RenderPipeline::RenderPipeline(WindowManager* wm)
{
    windowManager = wm;
}

glm::mat4 RenderPipeline::GetProjectionMatrix()
{
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    float aspect = width / (float)height;
    glm::mat4 Projection = glm::perspective(glm::radians(50.0f), aspect, 0.1f, 200.0f);
    return Projection;
}

void RenderPipeline::renderFrame(std::vector<GameObject*> objectsToRender, Camera* cam)
{
    // Get current frame buffer size.
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    glViewport(0, 0, width, height);

    // Clear framebuffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = width / (float)height;

    prog->bind();

    glm::mat4 P = this->GetProjectionMatrix();
    glm::mat4 V = cam->getCameraRotationMatrix();
    glm::mat4 M = glm::mat4(1);

    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, glm::value_ptr(V));
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, glm::value_ptr(M));

    glDisable(GL_DEPTH_TEST);

    this->skyboxMaterial->t_albedo->bind(prog->getUniform("Texture0"));
    this->skyboxMesh->draw(prog);
    this->skyboxMaterial->t_albedo->unbind();

    glEnable(GL_DEPTH_TEST);

    V = cam->getCameraViewMatrix();
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, glm::value_ptr(V));
    for (GameObject* obj : objectsToRender)
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



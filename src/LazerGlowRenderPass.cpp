#include "LazerGlowRenderPass.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "GameObject.h"
#include "MeshRenderer.h"

void LazerGlowRenderPass::init() {

    ResourceManager* rm = ResourceManager::getInstance();
    float width = rm->getNumericalValue("screenWidth");
    float height = rm->getNumericalValue("screenHeight");

    glGenFramebuffers(1, &frameBufferObject);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

    rm->addRenderTextureResource("lazerGlowOutput", colorBuffer);

    prog = std::make_shared<Program>();
    prog->setVerbose(true);
    prog->setShaderNames("../shaders/glow_vert.glsl", "../shaders/glow_frag.glsl");
    prog->init();
    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");
    prog->addAttribute("vertTex");
    prog->addUniform("P");
    prog->addUniform("V");
    prog->addUniform("M");
}

void LazerGlowRenderPass::execute(WindowManager* windowManager) {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);

    GLenum buffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffers);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get current frame buffer size.
    ResourceManager* rm = ResourceManager::getInstance();
    float width = rm->getNumericalValue("screenWidth");
    float height = rm->getNumericalValue("screenHeight");
    glViewport(0, 0, width, height);

    std::vector<GameObject*> renderables = *(std::vector<GameObject*> *)rm->getOther("renderables");


    Camera* cam = (Camera*)rm->getOther("activeCamera");
    glm::mat4 M, V, P;

    P = cam->getCameraProjectionMatrix();
    V = cam->getCameraViewMatrix();

    // Clear framebuffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog->bind();

    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, glm::value_ptr(V));

    for (GameObject* obj : renderables)
    {
        if (obj->name == "lazer") {
            M = obj->transform.genModelMatrix();

            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, glm::value_ptr(M));

            obj->getComponentByType<MeshRenderer>()->mesh->draw(prog);
        }
    }

    prog->unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);
}
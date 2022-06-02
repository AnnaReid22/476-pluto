#include "DeferredSamplingPass.h"

#include "ResourceManager.h"
#include "GameObject.h"
#include "Camera.h"
#include "MeshRenderer.h"

void DeferredSamplingPass::init()
{
    this->rm = rm->getInstance();
    float width = rm->getNumericalValue("screenWidth");
    float height = rm->getNumericalValue("screenHeight");

   
    glGenFramebuffers(1, &deferredFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);
    

    // - position buffer
    glGenTextures(1, &gBuffer);
    glBindTexture(GL_TEXTURE_2D, gBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer, 0);
    
    // - normal buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    
    // - color buffer
    glGenTextures(1, &gColor);
    glBindTexture(GL_TEXTURE_2D, gColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColor, 0);

    glGenRenderbuffers(1, &depthRenderBuffer);
    //set up depth necessary as rendering a mesh that needs depth test
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
    

    rm->addRenderTextureResource("gBuffer", gBuffer);
    rm->addRenderTextureResource("gNormal", gNormal);
    rm->addRenderTextureResource("gColor", gColor);

    prog = std::make_shared<Program>();
    prog->setVerbose(true);
    prog->setShaderNames("../shaders/defer_samples_vert.glsl", "../shaders/defer_samples_frag.glsl");
    prog->init();
    prog->addUniform("P");
    prog->addUniform("V");
    prog->addUniform("M");
    prog->addUniform("albedoMap");
    //prog->addUniform("normalMap");
    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");
    prog->addAttribute("vertTex");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredSamplingPass::execute(WindowManager* windowManager)
{
    glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);

    GLenum buffers[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, buffers);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Get current frame buffer size.
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    float aspect = width / (float)height;
    glViewport(0, 0, width, height);

    Camera* cam = (Camera*)rm->getOther("activeCamera");
    std::vector<GameObject*> renderables = *(std::vector<GameObject*> *)rm->getOther("renderables");

    glm::mat4 M, V, P;

    P = cam->getCameraProjectionMatrix();
    V = cam->getCameraViewMatrix();
    M = glm::mat4(1);

    prog->bind();

    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, glm::value_ptr(V));

    for (GameObject* obj : renderables)
    {
        if (obj->name == "lazer") {
            continue;
        }


        glm::mat4 Mchild = obj->transform.genModelMatrix();
        if (obj->parent)
        {
            //glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), 4.0f);

            M = obj->parentObj->transform.genModelMatrix() * obj->transform.genHierarchicalMatrix(); 
            //std::cout << "compare:" << obj << std::endl;
            //std::cout << "player compare: " << obj->parentObj << std::endl;;
        }
        else
        {
            M = Mchild;
        }


       // M = obj->transform.genModelMatrix();
        MeshRenderer* mr = obj->getComponentByType<MeshRenderer>();

        std::shared_ptr<Material> mat = mr->material;
        std::shared_ptr<Shape> mesh = mr->mesh;

        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, glm::value_ptr(M));

        mat->t_albedo->bind(prog->getUniform("albedoMap"));
        //mat->t_normal->bind(prog->getUniform("normalMap"));

        mesh->draw(prog);

        mat->t_albedo->unbind();
        //mat->t_normal->unbind();
    }

    prog->unbind();

    glDrawBuffers(1, buffers);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, gColor);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glGenerateMipmap(GL_TEXTURE_2D);
}
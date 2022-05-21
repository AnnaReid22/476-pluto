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
    // Initialize programs
  	depthProg = std::make_shared<Program>();
  	depthProg->setVerbose(true);
  	depthProg->setShaderNames("../shaders/depth_vert.glsl", "../shaders/depth_frag.glsl");
  	depthProg->init();

  	depthProgDebug = std::make_shared<Program>();
  	depthProgDebug->setVerbose(true);
  	depthProgDebug->setShaderNames("../shaders/depth_vertDebug.glsl", "../shaders/depth_fragDebug.glsl");
  	depthProgDebug->init();

  	shadowProg = std::make_shared<Program>();
  	shadowProg->setVerbose(true);
  	shadowProg->setShaderNames("../shaders/shadow_vert.glsl", "../shaders/shadow_frag.glsl");
  	shadowProg->init();

  	shadowDebugProg = std::make_shared<Program>();
  	shadowDebugProg->setVerbose(true);
  	shadowDebugProg->setShaderNames("../shaders/pass_vert.glsl", "../shaders/pass_texfrag.glsl");
  	shadowDebugProg->init();

    // Add uniform and attributes
  	depthProg->addUniform("LP");
  	depthProg->addUniform("LV");
  	depthProg->addUniform("M");
  	depthProg->addAttribute("vertPos");
  	depthProg->addAttribute("vertNor");
  	depthProg->addAttribute("vertTex");

  	depthProgDebug->addUniform("LP");
  	depthProgDebug->addUniform("LV");
  	depthProgDebug->addUniform("M");
  	depthProgDebug->addAttribute("vertPos");
  	depthProgDebug->addAttribute("vertNor");
  	depthProgDebug->addAttribute("vertTex");

  	shadowProg->addUniform("P");
  	shadowProg->addUniform("M");
  	shadowProg->addUniform("V");
    shadowProg->addUniform("LS");
  	shadowProg->addUniform("lightDir");
  	shadowProg->addAttribute("vertPos");
  	shadowProg->addAttribute("vertNor");
  	shadowProg->addAttribute("vertTex");
  	shadowProg->addUniform("Texture0");
  	shadowProg->addUniform("shadowDepth");

  	shadowDebugProg->addUniform("texBuf");
  	shadowDebugProg->addAttribute("vertPos");

    //generate the FBO for the shadow depth
  	glGenFramebuffers(1, &depthMapFBO);

    //generate the texture
  	glGenTextures(1, &depthMap);
  	glBindTexture(GL_TEXTURE_2D, depthMap);
  	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, S_WIDTH, S_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //bind with framebuffer's depth buffer
  	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  	glDrawBuffer(GL_NONE);
  	glReadBuffer(GL_NONE);
  	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowPass::execute(WindowManager * windowManager)
{
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
    //light lookat and y vector
    vec3 lightLA = vec3(0.0);
    vec3 lightUp = vec3(0, 1, 0);

    mat4 LO, LV, LSpace;
    if (SHADOW) {
    //set up light's depth map
  	 glViewport(0, 0, S_WIDTH, S_HEIGHT);
  	 glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  	 glClear(GL_DEPTH_BUFFER_BIT);
  	 glCullFace(GL_FRONT);

    //set up shadow shader and render the scene
    depthProg->bind();

    //light orthogonal view
    LO = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, -10.0f, 500.0f);
    glUniformMatrix4fv(depthProg->getUniform("LP"), 1, GL_FALSE, glm::value_ptr(LO));

    //light view
    LV = glm::lookAt(g_light, lightLA, lightUp);
    glUniformMatrix4fv(depthProg->getUniform("LV"), 1, GL_FALSE, glm::value_ptr(LV));
    draw(depthProg, 0);

    depthProg->unbind();
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (DEBUG_LIGHT) {	
  		if (GEOM_DEBUG) { 
  			depthProgDebug->bind();
            mat4 ortho = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, -10.0f, 500.0f);
            glUniformMatrix4fv(depthProgDebug->getUniform("LP"), 1, GL_FALSE, glm::value_ptr(ortho));
            mat4 camera = glm::lookAt(g_light, lightLA, lightUp);
            glUniformMatrix4fv(depthProgDebug->getUniform("LV"), 1, GL_FALSE, glm::value_ptr(camera));
  			draw(depthProgDebug, shadowProg->getUniform("Texture0"));
  			depthProgDebug->unbind();
  		} else {
  			shadowDebugProg->bind();
  			glActiveTexture(GL_TEXTURE0);
  			glBindTexture(GL_TEXTURE_2D, depthMap);
  			glUniform1i(shadowDebugProg->getUniform("texBuf"), 0);
  			glEnableVertexAttribArray(0);
  			// glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
  			// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
  			// glDrawArrays(GL_TRIANGLES, 0, 6);
  			// glDisableVertexAttribArray(0);
  			shadowDebugProg->unbind();
  		}
  	} else {
  		shadowProg->bind();
  		glActiveTexture(GL_TEXTURE1);
  		glBindTexture(GL_TEXTURE_2D, depthMap);
  		glUniform1i(shadowProg->getUniform("shadowDepth"), 1);
  		glUniform3f(shadowProg->getUniform("lightDir"), g_light.x, g_light.y, g_light.z);

        Camera* cam = (Camera*)rm->getOther("activeCamera");
        mat4 P = cam->getCameraProjectionMatrix();
        mat4 V = cam->getCameraViewMatrix();

        glUniformMatrix4fv(shadowProg->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P));
        glUniformMatrix4fv(shadowProg->getUniform("V"), 1, GL_FALSE, glm::value_ptr(V));

        LSpace = LO*LV;
        glUniformMatrix4fv(shadowProg->getUniform("LS"), 1, GL_FALSE, glm::value_ptr(LSpace));

  		draw(shadowProg, shadowProg->getUniform("Texture0"));
  		shadowProg->unbind();
  	}
}

void ShadowPass::draw(std::shared_ptr<Program> prog, GLint texID) 
{
    std::vector<GameObject*> renderables = *(std::vector<GameObject*> *)rm->getOther("renderables");
    mat4 M;
    for (GameObject* obj : renderables)
    {
        M = obj->transform.genModelMatrix();
        MeshRenderer* mr = obj->getComponentByType<MeshRenderer>();

        std::shared_ptr<Material> mat = mr->material;
        std::shared_ptr<Shape> mesh = mr->mesh;

        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, glm::value_ptr(M));

        mat->t_albedo->bind(prog->getUniform("albedoMap"));

        mesh->draw(prog);

        mat->t_albedo->unbind();
    }
}

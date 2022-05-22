#include "SkyboxRenderPass.h"
#include "ResourceManager.h"
#include "Camera.h"

void SkyboxRenderPass::init() {

    ResourceManager *rm = ResourceManager::getInstance();
    float width = rm->getNumericalValue("screenWidth");
    float height = rm->getNumericalValue("screenHeight");

    glGenFramebuffers(1, &frameBufferObject);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);

    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, colorBuffer, 0);

    rm->addRenderTextureResource("skyboxRenderTexture", colorBuffer);

		texSphere = std::make_shared<Shape>();
		texSphere->loadMesh("resources/texCube.obj");
		texSphere->resize();
		texSphere->init();

		sky_texture_albedo = std::make_shared<Texture>();
    sky_texture_albedo->setFilename("resources/spaceSkybox.jpg");
    sky_texture_albedo->init();
    sky_texture_albedo->setUnit(0);
    sky_texture_albedo->setWrapModes(GL_REPEAT, GL_REPEAT);

		sky_noise_albedo = std::make_shared<Texture>();
		sky_noise_albedo->setFilename("resources/twinkle_noise.jpg");
		sky_noise_albedo->init();
		sky_noise_albedo->setUnit(0);
		sky_noise_albedo->setWrapModes(GL_REPEAT, GL_REPEAT);

    prog = std::make_shared<Program>();
    prog->setVerbose(true);
    prog->setShaderNames("../shaders/sky_vert.glsl", "../shaders/sky_frag.glsl");
    prog->init();
    prog->addUniform("SkyMat");
    prog->addUniform("time");
    prog->addUniform("Texture0");
    prog->addUniform("TwinkleNoise");
    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");
    prog->addAttribute("vertTex");
}

void SkyboxRenderPass::execute(WindowManager* windowManager) {

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

    // Clear framebuffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = width / (float)height;

    Camera* cam = (Camera*)rm->getOther("activeCamera");

    prog->bind();

    glm::mat4 SkyMat = cam->getCameraProjectionMatrix() * cam->getCameraRotationMatrix();

    glUniformMatrix4fv(prog->getUniform("SkyMat"), 1, GL_FALSE, glm::value_ptr(SkyMat));
    glUniform1f(prog->getUniform("time"), Time::getInstance()->getGlobalTime());

    glDisable(GL_DEPTH_TEST);

    sky_texture_albedo->bind(prog->getUniform("Texture0"));
    sky_noise_albedo->bind(prog->getUniform("TwinkleNoise"));
    texSphere->draw(prog);
    sky_noise_albedo->unbind();
    sky_texture_albedo->unbind();

    glEnable(GL_DEPTH_TEST);

    prog->unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
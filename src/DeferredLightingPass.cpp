#include "DeferredLightingPass.h"
#include "Camera.h"

/**** geometry set up for a quad *****/
void DeferredLightingPass::initQuad() {
	//now set up a simple quad for rendering FBO
	glGenVertexArrays(1, &quad_VertexArrayID);
	glBindVertexArray(quad_VertexArrayID);

	static const GLfloat g_quad_vertex_buffer_data[] =
	{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};

	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
}

void DeferredLightingPass::init()
{
	this->rm = rm->getInstance();
	float width = rm->getNumericalValue("screenWidth");
	float height = rm->getNumericalValue("screenHeight");

	glGenFramebuffers(1, &lightingFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);

	// - color buffer
	glGenTextures(1, &gLightOutput);
	glBindTexture(GL_TEXTURE_2D, gLightOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gLightOutput, 0);

	rm->addRenderTextureResource("gLightOutput", gLightOutput);

    prog = std::make_shared<Program>();
    prog->setVerbose(true);
    prog->setShaderNames("../shaders/deferred_pass_vert.glsl", "../shaders/deferred_lighting_frag.glsl");
    prog->init();
    prog->addUniform("gBuffer");
    prog->addUniform("gNormal");
    prog->addUniform("gColor");
    prog->addUniform("lightPos");
	prog->addUniform("depthMap");
	prog->addUniform("LS");

	initQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void DeferredLightingPass::execute(WindowManager* windowManager)
{
	GLuint gBuffer = rm->getRenderTextureResource("gBuffer");
	GLuint gNormal = rm->getRenderTextureResource("gNormal");
	GLuint gColor = rm->getRenderTextureResource("gColor");
	GLuint depthMap = rm->getRenderTextureResource("shadowOutput");

	glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);

	GLenum buffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);

	int width, height;

	glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
	float aspect = width / (float)height;
	glViewport(0, 0, width, height);
	// Clear framebuffer.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	prog->bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gBuffer);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gColor);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glUniform1i(prog->getUniform("gBuffer"), 0);
		glUniform1i(prog->getUniform("gNormal"), 1);
		glUniform1i(prog->getUniform("gColor"), 2);
		glUniform3f(prog->getUniform("lightPos"), 0.0, 50.0, -100.0);
		glUniform1i(prog->getUniform("depthMap"), 3);

		glm::vec3 lightLA = glm::vec3(0.0, 0.0, -1000.0);
		glm::vec3 lightUp = glm::vec3(0, 1, 0);
		glm::mat4 LO, LV, LSpace;

		LO = glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, -500.0f, 500.0f);
		LV = glm::lookAt(glm::vec3(0.0, 50.0, 1000.0), lightLA, lightUp);

		LSpace = LO*LV;
		glUniformMatrix4fv(prog->getUniform("LS"), 1, GL_FALSE, glm::value_ptr(LSpace));

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
	prog->unbind();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, gLightOutput);
	glGenerateMipmap(GL_TEXTURE_2D);
}

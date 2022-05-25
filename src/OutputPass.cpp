#include "OutputPass.h"
#include "Camera.h"

/**** geometry set up for a quad *****/
void OutputPass::initQuad() {
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

void OutputPass::init()
{
	this->rm = rm->getInstance();
	float width = rm->getNumericalValue("screenWidth");
	float height = rm->getNumericalValue("screenHeight");

	prog = std::make_shared<Program>();
	prog->setVerbose(true);
	prog->setShaderNames("../shaders/deferred_pass_vert.glsl", "../shaders/output_frag.glsl");
	prog->init();
	prog->addUniform("gLightOutput");
	prog->addUniform("psColorOutput");
	prog->addUniform("psPositionOutput");
	prog->addUniform("shadowOutput");
	prog->addUniform("V");
	prog->addUniform("gBuffer");

	initQuad();
}

void OutputPass::execute(WindowManager* windowManager)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLuint gLightOutput = rm->getRenderTextureResource("gLightOutput");
	GLuint psColorOutput = rm->getRenderTextureResource("psColorOutput");
	GLuint psPositionOutput = rm->getRenderTextureResource("psPositionOutput");

	GLuint gBuffer = rm->getRenderTextureResource("gBuffer");
	GLuint shadowOutput = rm->getRenderTextureResource("shadowOutput");

	Camera* cam = (Camera*)rm->getOther("activeCamera");
    glm::mat4 V = cam->getCameraViewMatrix();

	int width, height;

	glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
	float aspect = width / (float)height;
	
	glViewport(0, 0, width, height);
	// Clear framebuffer.
	glClearColor(.12f, .34f, .56f, 1.0f);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	prog->bind();


		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, psColorOutput);
		// glUniform1i(prog->getUniform("psColorOutput"), 0);

		// glActiveTexture(GL_TEXTURE1);
		// glBindTexture(GL_TEXTURE_2D, gLightOutput);
		// glUniform1i(prog->getUniform("gLightOutput"), 1);

		// glActiveTexture(GL_TEXTURE2);
		// glBindTexture(GL_TEXTURE_2D, psPositionOutput);
		// glUniform1i(prog->getUniform("psPositionOutput"), 2);

		// glActiveTexture(GL_TEXTURE3);
		// glBindTexture(GL_TEXTURE_2D, gBuffer);
		// glUniform1i(prog->getUniform("gBuffer"), 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, shadowOutput);
		glUniform1i(prog->getUniform("shadowOutput"), 4);

		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, glm::value_ptr(V));

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
	prog->unbind();

}

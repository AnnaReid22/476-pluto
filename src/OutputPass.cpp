#include "OutputPass.h"

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

	initQuad();
}

void OutputPass::execute(WindowManager* windowManager)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLuint gLightOutput = rm->getRenderTextureResource("gLightOutput");

	int width, height;

	glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
	float aspect = width / (float)height;
	
	glViewport(0, 0, width, height);
	// Clear framebuffer.
	glClearColor(.12f, .34f, .56f, 1.0f);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	prog->bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gLightOutput);
		glUniform1i(prog->getUniform("gLightOutput"), 0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
	prog->unbind();

}

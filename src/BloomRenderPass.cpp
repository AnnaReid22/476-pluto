#include "BloomRenderPass.h"

/**** geometry set up for a quad *****/
void BloomRenderPass::initQuad() {
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


void BloomRenderPass::init()
{
    rm = rm->getInstance();
    float width = rm->getNumericalValue("screenWidth");
    float height = rm->getNumericalValue("screenHeight");

    glGenFramebuffers(2, blurFB);
    glGenTextures(2, blurPassTexture);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, blurFB[i]);
        glBindTexture(GL_TEXTURE_2D, blurPassTexture[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurPassTexture[i], 0
        );
    }

    glGenFramebuffers(1, &filterBrightFB);
    glBindFramebuffer(GL_FRAMEBUFFER, filterBrightFB);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurPassTexture[1], 0);

    rm->addRenderTextureResource("bloomColor", blurPassTexture[0]);

    filterBrightProg = std::make_shared<Program>();
    filterBrightProg->setVerbose(true);
    filterBrightProg->setShaderNames("../shaders/deferred_pass_vert.glsl", "../shaders/filter_bright.glsl");
    filterBrightProg->init();
    filterBrightProg->addUniform("inTexture");

    blurProg = std::make_shared<Program>();
    blurProg->setVerbose(true);
    blurProg->setShaderNames("../shaders/deferred_pass_vert.glsl", "../shaders/gauss_blur.glsl");
    blurProg->init();
    blurProg->addUniform("inTexture");
    blurProg->addUniform("horizontal");

    initQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomRenderPass::execute(WindowManager* windowManager)
{
    glBindFramebuffer(GL_FRAMEBUFFER, filterBrightFB);

    GLenum buffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffers);

    float width = rm->getNumericalValue("screenWidth");
    float height = rm->getNumericalValue("screenHeight");

    float aspect = width / (float)height;
    glViewport(0, 0, width, height);
    // Clear framebuffer.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint inTex = rm->getRenderTextureResource("psColorOutput");

    filterBrightProg->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inTex);
    glUniform1i(filterBrightProg->getUniform("inTexture"), 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    filterBrightProg->unbind();

    for (int i = 0; i < 64; i++)
    {
        int horizontal = i % 2;
        execute_blur_pass(horizontal);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomRenderPass::execute_blur_pass(bool horizontal)
{
    glBindFramebuffer(GL_FRAMEBUFFER, blurFB[horizontal]);
    blurProg->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blurPassTexture[!horizontal]);
    glUniform1i(blurProg->getUniform("inTexture"), 0);
    glUniform1i(blurProg->getUniform("horizontal"), horizontal);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    blurProg->unbind();
}

#pragma once
#include "GuiRenderPass.h"
#include "GameObject.h"
#include "Player.h"
#include "ResourceManager.h"

#include "GLSL.h"
#include "IShader.h"
#include "Program.h"
#include "Time.h"
#include <memory>

void addRect(std::vector<float> *rectBuf, float x, float y, float w, float h) {
    rectBuf->push_back(x);
    rectBuf->push_back(y);
    rectBuf->push_back(w);
    rectBuf->push_back(h);
}

void GuiRenderPass::init() {
    ResourceManager* rm = ResourceManager::getInstance();
    float width = rm->getNumericalValue("screenWidth");
    float height = rm->getNumericalValue("screenHeight");

    glGenFramebuffers(1, &frameBufferObject);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

    rm->addRenderTextureResource("guiColorOutput", colorBuffer);

    // Load program
    prog = std::make_shared<Program>();
    prog->setVerbose(true);
    prog->setShaderNames("../shaders/guiVert.glsl", "../shaders/guiFrag.glsl", "../shaders/guiGeom.glsl");
    prog->init();
    prog->addAttribute("rectArgs");
    prog->addUniform("aspect");
    prog->addUniform("rgb");
    prog->addUniform("texAlpha");
    prog->addUniform("tex");

    //Load textures
    this->winTex.setFilename("../resources/text/win.png");
    this->winTex.init();
    this->winTex.setUnit(0);
    this->winTex.setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    this->startTex.setFilename("../resources/text/start.png");
    this->startTex.init();
    this->startTex.setUnit(0);
    this->startTex.setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    this->deathTex.setFilename("../resources/text/death.png");
    this->deathTex.init();
    this->deathTex.setUnit(0);
    this->deathTex.setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    //Also initialize the shapes
    //push 7 orange circles for the lives
    //push 7 white outer circles for the lives

    std::vector<float> rectBuf;
    //Indicator of progress bar
    addRect(&rectBuf, 0.8f, 0.7f, 0.01f, 0.01f);
    addRect(&rectBuf, 0.8f, 0.7f, 0.0666f, 0.01f);
    //Top and bottom of progress bar
    addRect(&rectBuf, 0.8f, 0.7f, 0.1f, 0.01f);
    addRect(&rectBuf, 0.8f, -0.7f, 0.1f, 0.01f);
    //Outer Life Rings
    float ringX = -0.8f;
    float ringY = -0.6f;
    addRect(&rectBuf, ringX + 0.00f, ringY + 0.0f, 0.06f, -0.8f);
    addRect(&rectBuf, ringX + 0.08f, ringY + 0.0f, 0.06f, -0.8f);
    addRect(&rectBuf, ringX - 0.08f, ringY + 0.0f, 0.06f, -0.8f);
    addRect(&rectBuf, ringX + 0.04f, ringY + 0.123f, 0.06f, -0.8f);
    addRect(&rectBuf, ringX - 0.04f, ringY + 0.123f, 0.06f, -0.8f);
    addRect(&rectBuf, ringX + 0.04f, ringY - 0.123f, 0.06f, -0.8f);
    addRect(&rectBuf, ringX - 0.04f, ringY - 0.123f, 0.06f, -0.8f);
    //Points
    float pointsX = -0.93f;
    float pointsY = 0.9f;
    int numPoints = 100;
    for (int i = 0; i < numPoints; i += 2) {
        float dx = (float)i / 55;
        addRect(&rectBuf, pointsX + dx, pointsY, 0.014f, 0.014f);
        addRect(&rectBuf, pointsX + dx + 0.02f, pointsY - 0.03333f, 0.014f, 0.014f);
    }
    //Inner Life Rings
    addRect(&rectBuf, ringX + 0.00f, ringY + 0.0f, 0.035f, 0.035f);
    addRect(&rectBuf, ringX - 0.08f, ringY + 0.0f, 0.035f, 0.035f);
    addRect(&rectBuf, ringX - 0.04f, ringY + 0.123f, 0.035f, 0.035f);
    addRect(&rectBuf, ringX + 0.04f, ringY + 0.123f, 0.035f, 0.035f);
    addRect(&rectBuf, ringX + 0.08f, ringY + 0.0f, 0.035f, 0.035f);
    addRect(&rectBuf, ringX + 0.04f, ringY - 0.123f, 0.035f, 0.035f);
    addRect(&rectBuf, ringX - 0.04f, ringY - 0.123f, 0.035f, 0.035f);

    //Full Circle
    addRect(&rectBuf, 0.0f, 0.0f, 1.0f, 1.0f);

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    // Send the position array to the GPU
    glGenBuffers(1, &rectBufId);
    glBindBuffer(GL_ARRAY_BUFFER, rectBufId);
    glBufferData(GL_ARRAY_BUFFER, rectBuf.size() * sizeof(float), rectBuf.data(), GL_STATIC_DRAW);

    // Unbind the arrays
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    assert(glGetError() == GL_NO_ERROR);
    this->setGeometry(7, 100, 0.0f);
}


void GuiRenderPass::setGeometry(int lives, int score, float distToGoal) {
    this->numWhitePoints = 11 + score;
    if (this->numWhitePoints > 111) {
        this->numWhitePoints = 111;
    }else if (this->numWhitePoints < 11) {
        this->numWhitePoints = 11;
    }
    this->numOrangePoints = lives;
    if (this->numOrangePoints > 7) {
        this->numOrangePoints = 7;
    }else if (this->numOrangePoints < 0) {
        this->numOrangePoints = 0;
    }
    distToGoal = (distToGoal < 0) ? 0 : ((distToGoal > 1) ? 1 : distToGoal);
    float minY = -0.7f;
    float maxY = 0.7f;
    float topY = minY * (1 - distToGoal) + maxY * distToGoal;
    float midY = (minY + topY) * 0.5f;

    float indicatorData[8] = {
        0.8f, midY, 0.01f, topY-midY,
        0.8f, topY, 0.0666f, 0.01f,
    };
    glBindBuffer(GL_ARRAY_BUFFER, rectBufId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(float), indicatorData);
}

void GuiRenderPass::execute(WindowManager* windowManager) {
    // Setting the geometry based off of player attributes

    GameObject* player = (GameObject*)ResourceManager::getInstance()->getOther("player_game_object");
    int score = ResourceManager::getInstance()->getNumericalValue("score");
    float numLives = player->getComponentByType<Player>()->numLives;
    float progress = player->transform.position.z / -1000.0f;
    this->setGeometry(numLives, score, progress);

    // Rendering stuff
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);

    GLenum buffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, buffers);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get current frame buffer size.
    ResourceManager* rm = ResourceManager::getInstance();
    float width = rm->getNumericalValue("screenWidth");
    float height = rm->getNumericalValue("screenHeight");
    float aspect = width / height;
    glViewport(0, 0, width, height);

    // Clear framebuffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind Program
    prog->bind();

    // Send uniforms
    glUniform1f(prog->getUniform("aspect"), aspect);

    // Bind vertex array object
    glBindVertexArray(vaoID);


    // Bind rect buffer
    GLuint rectArgs = prog->getAttribute("rectArgs");
    GLSL::enableVertexAttribArray(rectArgs);
    glBindBuffer(GL_ARRAY_BUFFER, rectBufId);
    glVertexAttribPointer(rectArgs, 4, GL_FLOAT, GL_FALSE, 0, (const void*)0);

    //Draw
    //Draw white artifacts
    glUniform1f(prog->getUniform("texAlpha"), -1.0f);
    glUniform3f(prog->getUniform("rgb"), 1.0f, 1.0f, 1.0f);
    glDrawArrays(GL_POINTS, 0, this->numWhitePoints);

    //Draw orange artifacts
    glUniform1f(prog->getUniform("texAlpha"), -1.0f);
    glUniform3f(prog->getUniform("rgb"), 0.71764f, 0.46666f, 0.1607843f);
    glDrawArrays(GL_POINTS, 111, this->numOrangePoints);

    //Draw text
    float curTime = Time::getInstance()->getGlobalTime();
    float startTime = rm->getNumericalValue("guiStartTime");
    if (startTime < 0) {
        rm->addNumericalValue("guiStartTime", curTime);
    }
    startTime = startTime < 0 ? -1.0f : curTime - startTime;
    float winTime = rm->getNumericalValue("guiWinTime");
    winTime = winTime < 0 ? -1.0f : curTime - winTime;
    float deathTime = rm->getNumericalValue("guiDeathTime");
    deathTime = deathTime < 0 ? -1.0f : curTime - deathTime;

    bool drawText = false;

    float pi = 3.14159;

    if (startTime >= 0.0f && startTime <= pi) {
        this->startTex.bind(prog->getUniform("tex"));
        glUniform1f(prog->getUniform("texAlpha"), sqrt(abs(sin(startTime))));
        glDrawArrays(GL_POINTS, 118, 1);
    }
    else if (winTime >= 0.0f && winTime <= pi) {
        this->winTex.bind(prog->getUniform("tex"));
        glUniform1f(prog->getUniform("texAlpha"), sqrt(abs(sin(winTime))));
        glDrawArrays(GL_POINTS, 118, 1);
    }
    else if (deathTime >= 0.0f && deathTime <= pi) {
        this->deathTex.bind(prog->getUniform("tex"));
        glUniform1f(prog->getUniform("texAlpha"), sqrt(abs(sin(deathTime))));
        glDrawArrays(GL_POINTS, 118, 1);
    }
    
    // For testing purposes
    //if (InputManager::getInstance()->GetKey(GLFW_KEY_0)) {
    //    rm->addNumericalValue("guiDeathTime", curTime);
    //    rm->addNumericalValue("guiWinTime", curTime);
    //}


    // Disable and unbind
    GLSL::disableVertexAttribArray(rectArgs);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind Program
    prog->unbind();

    assert(glGetError() == GL_NO_ERROR);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    assert(glGetError() == GL_NO_ERROR);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);
}
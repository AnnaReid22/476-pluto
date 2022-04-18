#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Time
{
private:
    static Time* instance;

    double g_Time;
    double frametime;

    Time() {
        g_Time = glfwGetTime();
        frametime = 0;
    }

public:
    static Time* getInstance() {
        if (!instance)
            instance = new Time;
        return instance;
    }

    void updateTime() {
        double lastTime = g_Time;
        this->g_Time = glfwGetTime();
        this->frametime = g_Time - lastTime;
    }

    double getFrametime() {
        return this->frametime;
    }

    double getGlobalTime() {
        return this->g_Time;
    }
};

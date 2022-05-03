#pragma once

#include <unordered_map>
#include "Time.h"
#include "WindowManager.h"

typedef struct _key_status {
    bool isPressed;
    double timePressed;
} KeyStatus;

class InputManager
{
private:
    static InputManager* instance;

    Time* time;

    std::unordered_map<int, KeyStatus> keys;

    InputManager() {
        time = time->getInstance();
    };

    void InsertDefaultKeyStatusForKey(int key);

public:

    static InputManager* getInstance()
    {
        if (!instance)
            instance = new InputManager;
        return instance;
    }

    // Gets whether key is pressed regardless of timing
    bool GetKey(int key);

    // Gets whether key was pressed this frame
    bool GetKeyDown(int key);

    // Gets whether key was released this frame
    bool GetKeyUp(int key);

    void KeyPressed(int key);
    void KeyReleased(int key);

    void GetMousePos(double* xpos, double* ypos);

    WindowManager* windowManager;

};


#include "InputManager.h"

void InputManager::InsertDefaultKeyStatusForKey(int key)
{
    KeyStatus newKeyStatus;
    newKeyStatus.isPressed = false;
    newKeyStatus.timePressed = -1;

    keys[key] = newKeyStatus;
}

bool InputManager::GetKey(int key)
{
    auto status = keys.find(key);

    if (status != keys.end())
    {
        return status->second.isPressed;
    }
    else
    {
        InsertDefaultKeyStatusForKey(key);
        return false;
    }
}

bool InputManager::GetKeyDown(int key)
{
    double currTime = time->getGlobalTime();

    auto status = keys.find(key);

    if (status != keys.end())
    {
        return (status->second.isPressed) && (currTime == status->second.timePressed);
    }
    else
    {
        InsertDefaultKeyStatusForKey(key);
        return false;
    }
}

bool InputManager::GetKeyUp(int key)
{
    double currTime = time->getGlobalTime();

    auto status = keys.find(key);

    if (status != keys.end())
    {
        return (!(status->second.isPressed)) && (currTime == status->second.timePressed);
    }
    else
    {
        InsertDefaultKeyStatusForKey(key);
        return true;
    }
}

void InputManager::KeyPressed(int key)
{
    double currTime = time->getGlobalTime();

    keys[key] = { true, currTime };
}

void InputManager::KeyReleased(int key)
{
    keys[key] = { false, -1 };
}

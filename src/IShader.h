#pragma once

#include "WindowManager.h"

class IShader {
public:
    virtual void init() = 0;
    virtual void execute(WindowManager * windowManager) = 0;
};

//subclass of ishader that has init and execute
//forwardrenderpass example
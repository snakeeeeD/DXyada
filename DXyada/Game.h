#pragma once
#include "input.h"
#include <Windows.h>

class Game {
public:
    void Init(HWND hWnd);
    void Update();
    void Draw();
    void UnInit();
};

#pragma once
#include "input.h"
#include <Windows.h>
class Sound;
extern Sound* g_sound;
class Game {
public:
    void Init(HWND hWnd);
    void Update();
    void Draw();
    void UnInit();
};

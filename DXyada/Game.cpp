#include "Game.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "sound.h"
SceneManager g_sceneManager;
//Sound* g_sound = nullptr;
void Game::Init(HWND hWnd) {
    RendererInit(hWnd);
    g_sceneManager.Init(SCENE_TITLE);
    g_sound = new Sound();
    g_sound->Init();
}

void Game::Update() {
    g_sceneManager.Update();
}

void Game::Draw() {
    RendererDrawStart();
    g_sceneManager.Draw();
    RendererDrawEnd();
}

void Game::UnInit() {
    g_sceneManager.UnInit();
    RendererUninit();
    if (g_sound)
    {
        g_sound->Uninit();
        delete g_sound;
        g_sound = nullptr;
    }
}

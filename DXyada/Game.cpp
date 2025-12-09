#include "Game.h"
#include "SceneManager.h"
#include "Renderer.h"

SceneManager g_sceneManager;

void Game::Init(HWND hWnd) {
    RendererInit(hWnd);
    g_sceneManager.Init(SCENE_TITLE);
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
}

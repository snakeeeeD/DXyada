#include "SceneManager.h"

void SceneManager::Init(SceneID id)
{
    m_currentID = id;

    switch (id) {
    case SCENE_TITLE: m_currentScene = new SceneTitle(); break;
    case SCENE_GAME: m_currentScene = new SceneGame(); break;
    case SCENE_GAMEOVER: m_currentScene = new SceneGameOver(); break;
    }

    if (m_currentScene)
        m_currentScene->Init();
}

void SceneManager::Update()
{
    if (m_currentScene)
        m_currentScene->Update(*this);
}

void SceneManager::Draw()
{
    if (m_currentScene)
        m_currentScene->Draw();
}

void SceneManager::UnInit()
{
    if (m_currentScene) {
        m_currentScene->UnInit();
        delete m_currentScene;
        m_currentScene = nullptr;
    }
}

void SceneManager::ChangeScene(SceneID id)
{
    // まず安全に古いシーンを削除
    if (m_currentScene) {
        m_currentScene->UnInit();
        delete m_currentScene;
        m_currentScene = nullptr;
    }

    // 新しいシーンを作成
    m_currentID = id;
    switch (id) {
    case SCENE_TITLE: m_currentScene = new SceneTitle(); break;
    case SCENE_GAME: m_currentScene = new SceneGame(); break;
    case SCENE_GAMEOVER: m_currentScene = new SceneGameOver(); break;
    }

    if (m_currentScene)
        m_currentScene->Init();
}

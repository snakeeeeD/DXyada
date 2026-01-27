#include "SceneManager.h"

void SceneManager::Init(SceneID id)
{
    m_currentID = id;

    switch (id)
    {
    case SCENE_TITLE:  m_currentScene = new SceneTitle(); break;
    case SCENE_LOAD:   m_currentScene = new SceneLoad(SCENE_GAME); break; // —áFƒ[ƒhŒã‚ÉSelect‚Ö
    case SCENE_SELECT: m_currentScene = new SceneSelect(); break;
    case SCENE_GAME:   m_currentScene = new SceneGame(m_selectedStage); break;
    default:           m_currentScene = new SceneTitle(); break;
    }

    if (m_currentScene) m_currentScene->Init();
}

void SceneManager::Update()
{
    if (m_currentScene) m_currentScene->Update(*this);
}

void SceneManager::Draw()
{
    if (m_currentScene) m_currentScene->Draw();
}

void SceneManager::UnInit()
{
    if (m_currentScene)
    {
        m_currentScene->UnInit();
        delete m_currentScene;
        m_currentScene = nullptr;
    }
}

void SceneManager::ChangeScene(SceneID id)
{
    if (m_currentScene)
    {
        m_currentScene->UnInit();
        delete m_currentScene;
        m_currentScene = nullptr;
    }

    m_currentID = id;

    switch (id)
    {
    case SCENE_TITLE:  m_currentScene = new SceneTitle(); break;
    case SCENE_LOAD:   m_currentScene = new SceneLoad(SCENE_GAME); break;
    case SCENE_SELECT: m_currentScene = new SceneSelect(); break;
    case SCENE_GAME:   m_currentScene = new SceneGame(m_selectedStage); break;
    default:           m_currentScene = new SceneTitle(); break;
    }

    if (m_currentScene) m_currentScene->Init();
}

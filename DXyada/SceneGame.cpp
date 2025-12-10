#include "SceneManager.h" 
#include "SceneGame.h"
#include "Input.h"

extern Input input;

void SceneGame::Init() {
    m_stageManager.LoadStage(m_stage);

    m_collision = new CollisionManager();

    m_player.Init();

    for (auto& plat : m_stageManager.GetPlatforms()) {
        m_collision->AddStatic(plat.GetObject());
        m_collision->SetTag(plat.GetObject(), ColliderTag::Platform);
    }
    for (auto& enemy : m_stageManager.GetEnemy()) {
        m_collision->AddStatic(enemy.GetObject());
        m_collision->SetTag(enemy.GetObject(), ColliderTag::Enemy);
    }

    m_collision->AddDynamic(m_player.GetObject());
    m_collision->SetTag(m_player.GetObject(), ColliderTag::Player);
}

void SceneGame::Update(SceneManager& mgr) {

    m_collision->SetSceneManager(&mgr);

    if (input.GetKeyTrigger(VK_ESCAPE)) {
        mgr.ChangeScene(SCENE_TITLE);
        return;
    }

    m_player.Update(1.0f / 240.0f, m_stageManager.GetPlatforms(), m_stageManager.GetEnemy());
    m_camera.Update(m_player.GetObject()->GetPos());
    m_stageManager.Update();
    m_collision->CheckAll();
}

void SceneGame::Draw() {
    m_stageManager.Draw();
    m_player.Draw();
}

void SceneGame::UnInit() {
    m_stageManager.Uninit();
    m_player.Uninit();
}


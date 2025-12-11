#include "SceneGame.h"
#include "SceneManager.h"
#include "Input.h"

extern Input input;

<<<<<<< HEAD
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

=======
SceneGame::SceneGame(int stageNum)
{
    m_stageNumber = stageNum;
>>>>>>> 6e89df0d86a05429919e9271224159caaad8c44b
}

void SceneGame::Init()
{
    // 選択されたステージ番号でロード
    m_stageManager.LoadStage(m_stageNumber);
}

void SceneGame::Update(SceneManager& mgr)
{
    // エスケープでタイトルへ
    if (input.GetKeyTrigger(VK_ESCAPE)) {
        mgr.ChangeScene(SCENE_TITLE);
        return;
    }

    // ステージ更新
    m_stageManager.Update();

    // プレイヤー死亡判定
    StageBase* stage = m_stageManager.GetStage();
    if (stage && stage->IsPlayerDead()) {
        //mgr.ChangeScene(SCENE_GAMEOVER);
        return;
    }
}

void SceneGame::Draw()
{
    m_stageManager.Draw();
}

void SceneGame::UnInit()
{
    m_stageManager.Uninit();
}


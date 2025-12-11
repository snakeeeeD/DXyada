#include "SceneGame.h"
#include "SceneManager.h"
#include "Input.h"

extern Input input;

SceneGame::SceneGame(int stageNum)
{
    m_stageNumber = stageNum;
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


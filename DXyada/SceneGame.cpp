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
    input.Update();

    switch (m_state)
    {
    case GameState::Playing:
        UpdatePlaying(mgr);
        break;

    case GameState::Pouse:
        UpdatePouse(mgr);
        break;

    case GameState::Result:
        UpdatePouse(mgr);
        break;

    case GameState::GameOver:
        UpdateGameOver(mgr);
        break;
    }
}

void SceneGame::UpdatePlaying(SceneManager& mgr) {

        //エスケープでタイトルに
        if (input.GetKeyTrigger(VK_ESCAPE)) {
            m_state = GameState::Pouse;
        }

        if (input.GetKeyTrigger(VK_UP)) {
            m_state = GameState::GameOver;
        }

        // ステージ更新
        m_stageManager.Update();

        // プレイヤー死亡判定
        StageBase* stage = m_stageManager.GetStage();
        if (stage && stage->IsPlayerDead()) {
            
            return;
        }
}
void SceneGame::UpdatePouse(SceneManager& mgr) {
    if (input.GetKeyTrigger(VK_ESCAPE)) {
        m_state = GameState::Playing;
    }
    if (input.GetKeyTrigger(VK_RETURN))
    {
        mgr.ChangeScene(SCENE_GAME);
    }
    if (input.GetKeyTrigger(VK_SPACE))
    {
        mgr.ChangeScene(SCENE_SELECT);
    }
}
void SceneGame::UpdateResult(SceneManager& mgr) {

}
void SceneGame::UpdateGameOver(SceneManager& mgr) {
    if (input.GetKeyTrigger(VK_RETURN))
    {
        mgr.ChangeScene(SCENE_GAME);
    }

    // Esc でタイトルに戻る
    if (input.GetKeyTrigger(VK_ESCAPE))
    {
        mgr.ChangeScene(SCENE_TITLE);
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


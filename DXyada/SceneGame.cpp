#include "SceneGame.h"
#include "SceneManager.h"
#include "Input.h"

extern Input input;

extern DirectX::XMFLOAT3 g_cameraPos;

#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

void CheckAllPads()
{
    XINPUT_STATE state{};
    for (DWORD i = 0; i < 4; i++)
    {
        if (XInputGetState(i, &state) == ERROR_SUCCESS)
        {
            char buf[64];
            sprintf_s(buf, "Pad Connected: index %d", i);
            MessageBoxA(NULL, buf, "XInput", MB_OK);
        }
    }
}

SceneGame::SceneGame(int stageNum)
{
    m_stageNumber = stageNum;
}

void SceneGame::Init()
{
    CheckAllPads();
    // 選択されたステージ番号でロード
    m_stageManager.LoadStage(m_stageNumber);


    Pouse_BackGround.Init();
    Pouse_BackGround.AddTexture("asset/Pouse.png");

    GameOver_BackGround.Init();
    GameOver_BackGround.AddTexture("asset/GameOver.png");
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
    GameOver_BackGround.SetSize(0, 0, 0);
    Pouse_BackGround.SetSize(0, 0, 0);

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
            m_state = GameState::GameOver;
            return;
        }
}
void SceneGame::UpdatePouse(SceneManager& mgr) {
    Pouse_BackGround.SetSize(1920, 1080, 0);
    Pouse_BackGround.SetColor(1, 1, 1, 0.3);
    Pouse_BackGround.SetPos(g_cameraPos.x, g_cameraPos.y, 0);

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
    GameOver_BackGround.SetSize(1920, 1080, 0);
    GameOver_BackGround.SetPos(g_cameraPos.x, g_cameraPos.y, 0);

    if (input.GetKeyTrigger(VK_RETURN))
    {
        mgr.ChangeScene(SCENE_GAME);
    }

    // Esc でステージ選択に戻る
    if (input.GetKeyTrigger(VK_ESCAPE))
    {
        mgr.ChangeScene(SCENE_SELECT);
    }
}
void SceneGame::Draw()
{
    m_stageManager.Draw();

    Pouse_BackGround.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );

    GameOver_BackGround.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
}

void SceneGame::UnInit()
{
    m_stageManager.Uninit();
}


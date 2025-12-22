#include "SceneGame.h"
#include "SceneManager.h"
#include "Input.h"
#include <iostream>

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

    m_cursor.Init();
    m_cursor.AddTexture("asset/cursor.png");
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

    m_cursor.SetPos(0, 0, 0);
    m_cursor.SetSize(0, 0, 0);
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
    Pouse_BackGround.SetColor(1, 1, 1, 1);
    Pouse_BackGround.SetPos(g_cameraPos.x, g_cameraPos.y, 0);
   
    m_cursor.SetSize(m_size, m_size, 0);

    input.Update();
    if (input.GetKeyPress(VK_UP) || input.GetButtonTrigger(XINPUT_UP)) {
        if (m_cursorNum > 0)
        {
            m_cursorNum-=1;
        }
    }
    if (input.GetKeyPress(VK_DOWN) || input.GetButtonPress(XINPUT_DOWN)) {
        if (m_cursorNum < 2)
        {
            m_cursorNum+=1;
        }
        
    }

    switch (m_cursorNum)
    {
    case 0:
        m_cursor.SetPos(g_cameraPos.x + 30, g_cameraPos.y + 225, 0);
            if (input.GetKeyPress(VK_RETURN) || input.GetButtonPress(XINPUT_A)) {
                m_state = GameState::Playing;
            }
        break;

    case 1:
        m_cursor.SetPos(g_cameraPos.x + 30, g_cameraPos.y, 0);
        if (input.GetKeyTrigger(VK_RETURN))
        {
            mgr.ChangeScene(SCENE_GAME);
        }
        break;

    case 2:
        m_cursor.SetPos(g_cameraPos.x + 30, g_cameraPos.y - 225, 0);
        if (input.GetKeyTrigger(VK_RETURN))
        {
            mgr.ChangeScene(SCENE_SELECT);
        }
        break;
    }

    if (m_size == SizeMin) {
        Big = true;
        Small = false;
    }
    if (m_size == SizeMax) {
        Big = false;
        Small = true;
    }

    if (Big == true) {
        if (deltaTime == 5) {
            m_size += 1;
            deltaTime = 0;
        }
        deltaTime++;
    }
    if (Small == true) {
        if (deltaTime == 5) {
            m_size -= 1;
            deltaTime = 0;
        }
        deltaTime++;
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
    m_cursor.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
}

void SceneGame::UnInit()
{
    Pouse_BackGround.UnInit();
    GameOver_BackGround.UnInit();
    m_stageManager.Uninit();
    m_cursor.UnInit();
}


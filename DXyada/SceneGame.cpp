#include "SceneGame.h"
#include "SceneManager.h"
#include "Input.h"
#include <iostream>

extern Input input;

extern DirectX::XMFLOAT3 g_cameraPos;
DirectX::XMFLOAT3 g_respawnPos;

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
  //  CheckAllPads();
    // 選択されたステージ番号でロード
    m_stageManager.LoadStage(m_stageNumber);


    Pouse_BackGround.Init();
    Pouse_BackGround.AddTexture("asset/UI/Pouse_Back.png");

    GameOver_BackGround.Init();
    GameOver_BackGround.AddTexture("asset/UI/GameOver.png");

    m_cursor.Init();
    m_cursor.AddTexture("asset/UI/cursor.png");

    
    m_Buttonretry.Init();
    m_Buttonretry.AddTexture("asset/UI/Button_Retry.png");

    m_Buttoncontinue.Init();
    m_Buttoncontinue.AddTexture("asset/UI/Button_Continue.png");

    m_ButtonStageselect.Init();
    m_ButtonStageselect.AddTexture("asset/UI/Button_StageSelect.png");
        
        
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

    m_Buttonretry.SetPos(0, 0, 0);
    m_Buttonretry.SetSize(0, 0, 0);

    m_Buttoncontinue.SetPos(0, 0, 0);
    m_Buttoncontinue.SetSize(0, 0, 0);

    m_ButtonStageselect.SetPos(0, 0, 0);
    m_ButtonStageselect.SetSize(0, 0, 0);

        //エスケープでポーズに、コントローラーはスタートで
        if (input.GetKeyTrigger(VK_ESCAPE) || input.GetButtonTrigger(XINPUT_START)) {
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

    
    m_Buttoncontinue.SetColor(1, 1, 1, 1);
    m_Buttoncontinue.SetPos(g_cameraPos.x + 320, g_cameraPos.y + 145, 0);

    
    m_Buttonretry.SetColor(1, 1, 1, 1);
    m_Buttonretry.SetPos(g_cameraPos.x+320, g_cameraPos.y -90, 0);

  

    m_ButtonStageselect.SetColor(1, 1, 1, 1);
    m_ButtonStageselect.SetPos(g_cameraPos.x + 320, g_cameraPos.y - 320, 0);
   
    m_cursor.SetSize(m_size, m_size, 0);

    if (input.GetKeyTrigger(VK_UP) || input.GetButtonTrigger(XINPUT_UP)) {
        if (m_cursorNum > 0)
        {
            m_cursorNum-=1;
        }
    }
    if (input.GetKeyTrigger(VK_DOWN) || input.GetButtonTrigger(XINPUT_DOWN)) {
        if (m_cursorNum < 2)
        {
            m_cursorNum+=1;
        } 
    }

    switch (m_cursorNum)
    {
    case 0:
        m_cursor.SetPos(g_cameraPos.x + 30, g_cameraPos.y + 225, 0);
        m_Buttoncontinue.SetSize(670, 185, 0);
        m_Buttonretry.SetSize(572, 160, 0);
        m_ButtonStageselect.SetSize(572, 160, 0);
            if (input.GetKeyTrigger(VK_RETURN) || input.GetButtonTrigger(XINPUT_A)) {
                m_state = GameState::Playing;
            }
        break;

    case 1:
        m_cursor.SetPos(g_cameraPos.x + 30, g_cameraPos.y-5, 0);
        m_Buttoncontinue.SetSize(572, 160, 0);
        m_Buttonretry.SetSize(670, 185, 0);
        m_ButtonStageselect.SetSize(572, 160, 0);
        if (input.GetKeyTrigger(VK_RETURN) || input.GetButtonTrigger(XINPUT_A))
        {
            mgr.ChangeScene(SCENE_GAME);
        }
        break;

    case 2:
        m_cursor.SetPos(g_cameraPos.x + 30, g_cameraPos.y - 235, 0);
        m_Buttoncontinue.SetSize(572, 160, 0);
        m_Buttonretry.SetSize(572, 160, 0);
        m_ButtonStageselect.SetSize(670, 185, 0);
        if (input.GetKeyTrigger(VK_RETURN) || input.GetButtonTrigger(XINPUT_A))
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

    m_Buttonretry.SetColor(1, 1, 1, 1);
    m_Buttonretry.SetPos(g_cameraPos.x - 420, g_cameraPos.y - 335, 0);

    m_ButtonStageselect.SetColor(1, 1, 1, 1);
    m_ButtonStageselect.SetPos(g_cameraPos.x + 420, g_cameraPos.y - 335, 0);

    m_cursor.SetSize(m_size, m_size, 0);

    if (input.GetKeyTrigger(VK_LEFT) || input.GetButtonTrigger(XINPUT_LEFT)) {
            m_cursorNum = 0;
    }
    if (input.GetKeyTrigger(VK_RIGHT) || input.GetButtonTrigger(XINPUT_RIGHT)) {
            m_cursorNum = 1;
    }

    switch (m_cursorNum)
    {
    case 0:
        m_cursor.SetPos(g_cameraPos.x - 700, g_cameraPos.y - 240, 0);
        m_Buttonretry.SetSize(670, 185, 0);
        m_ButtonStageselect.SetSize(572, 160, 0);
        if (input.GetKeyTrigger(VK_RETURN) || input.GetButtonTrigger(XINPUT_A))
        {
            mgr.ChangeScene(SCENE_GAME);
        }
        break;

    case 1:
        m_cursor.SetPos(g_cameraPos.x + 130, g_cameraPos.y - 240, 0);
        m_Buttonretry.SetSize(572, 160, 0);
        m_ButtonStageselect.SetSize(670, 185, 0);
        if (input.GetKeyTrigger(VK_RETURN) || input.GetButtonTrigger(XINPUT_A))
        {
            mgr.ChangeScene(SCENE_SELECT);
        }
        break;
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

    m_Buttonretry.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );

    m_Buttoncontinue.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );

    m_ButtonStageselect.Draw(
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
    m_Buttonretry.UnInit();
    m_Buttoncontinue.UnInit();
    m_ButtonStageselect.UnInit();
}



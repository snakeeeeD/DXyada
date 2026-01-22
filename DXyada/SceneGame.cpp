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

    m_Star1.Init();
    m_Star2.Init();
    m_Star3.Init();
    m_Star1.AddTexture("asset/UI/GrayStar.png");
    m_Star2.AddTexture("asset/UI/GrayStar.png");
    m_Star3.AddTexture("asset/UI/GrayStar.png");
    m_Star1.AddTexture("asset/UI/Star.png");
    m_Star2.AddTexture("asset/UI/Star.png");
    m_Star3.AddTexture("asset/UI/Star.png");

    m_Buttonretry.Init();
    m_Buttonretry.AddTexture("asset/UI/Button_Retry.png");

    m_Buttoncontinue.Init();
    m_Buttoncontinue.AddTexture("asset/UI/Button_Continue.png");

    m_ButtonStageselect.Init();
    m_ButtonStageselect.AddTexture("asset/UI/Button_StageSelect.png");

    m_ResultUI.Init();
    m_ResultUI.AddTexture("asset/UI/クリア画面.png");
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
        UpdateResult(mgr);
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
        BeginResult(mgr);
        return;
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

    DirectX::XMFLOAT2 leftStick = input.GetLeftAnalogStick();// 移動専用
    const float moveThreshold = 0.5f;  //左スティックのデッドゾーン

    if (leftStick.y > moveThreshold)
    {
        m_stickNow = 1;
    }
    else if (leftStick.y < -moveThreshold)
    {
        m_stickNow = 2;
    }
    else
    {
        m_stickNow = 0;
    }

    //左スティック判定
    bool leftStickUpTrigger = (m_stickNow == 1) && !m_prevLeftStick;
    bool leftStickDownTrigger = (m_stickNow == 2) && !m_prevLeftStick;

    Pouse_BackGround.SetSize(1920, 1080, 0);
    Pouse_BackGround.SetColor(1, 1, 1, 1);
    Pouse_BackGround.SetPos(g_cameraPos.x, g_cameraPos.y, 0);


    m_Buttoncontinue.SetColor(1, 1, 1, 1);
    m_Buttoncontinue.SetPos(g_cameraPos.x + 320, g_cameraPos.y + 145, 0);


    m_Buttonretry.SetColor(1, 1, 1, 1);
    m_Buttonretry.SetPos(g_cameraPos.x + 320, g_cameraPos.y - 90, 0);



    m_ButtonStageselect.SetColor(1, 1, 1, 1);
    m_ButtonStageselect.SetPos(g_cameraPos.x + 320, g_cameraPos.y - 320, 0);

    m_cursor.SetSize(m_size, m_size, 0);

    if (input.GetButtonTrigger(XINPUT_START) || input.GetButtonTrigger(XINPUT_B))
    {
        m_state = GameState::Playing;
    }

    if (input.GetKeyTrigger(VK_UP) ||
        input.GetButtonTrigger(XINPUT_UP) ||
        leftStickUpTrigger
        ) {
        if (m_cursorNum > 0)
        {
            m_cursorNum -= 1;
        }
    }
    if (input.GetKeyTrigger(VK_DOWN) ||
        input.GetButtonTrigger(XINPUT_DOWN) ||
        leftStickDownTrigger) {
        if (m_cursorNum < 2)
        {
            m_cursorNum += 1;
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
        m_cursor.SetPos(g_cameraPos.x + 30, g_cameraPos.y - 5, 0);
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

    m_prevLeftStick = m_stickNow;

}

static float EaseInOut(float x)
{
    // 0..1 → なめらか（簡易）
    return x * x * (3.0f - 2.0f * x);
}

DirectX::XMFLOAT3 SceneGame::GetResultStartCamPos(int stageNum) const
{
    // “スタート地点” を固定値で返す例（あなたの希望に合わせて数値管理）
    // ステージ毎に調整してOK
    switch (stageNum)
    {
    default:
    case 0: return { 0, 0, 0 };
    case 1: return { 0, 0, 0 };
    case 2: return { 0, 0, 0 };
    }
}

DirectX::XMFLOAT3 SceneGame::GetResultGoalCamPos(int stageNum) const
{
    // “ゴール地点” を固定値で返す例
    switch (stageNum)
    {
    default:
    case 0: return { 8000, -300, 0 };
    case 1: return { 10600, -300, 0 };
    case 2: return { 14000, -300, 0 };
    }
}

void SceneGame::BeginResult(SceneManager& mgr)
{
    m_resultCam.active = true;
    m_resultCam.t = 0.0f;
    m_resultCam.duration = 8.0f; // 好きに

    m_resultCam.start = GetResultStartCamPos(m_stageNumber);
    m_resultCam.goal = GetResultGoalCamPos(m_stageNumber);

    // 演出開始時点でカメラをスタートへ
    g_cameraPos = m_resultCam.start;

    m_state = GameState::Result;
}

void SceneGame::UpdateResult(SceneManager& mgr) {
    const float dt = 1.0f / 60.0f;
    m_resultCam.t += dt;

    float x = (m_resultCam.duration > 0.0f) ? (m_resultCam.t / m_resultCam.duration) : 1.0f;
    if (x < 0.0f) x = 0.0f;
    if (x > 1.0f) x = 1.0f;

    float s = EaseInOut(x);

    g_cameraPos.x = m_resultCam.start.x + (m_resultCam.goal.x - m_resultCam.start.x) * s;
    g_cameraPos.y = m_resultCam.start.y + (m_resultCam.goal.y - m_resultCam.start.y) * s;
    g_cameraPos.z = 0.0f;

    m_Star1.SetPos(g_cameraPos.x + 200, g_cameraPos.y + -300, 0);
    m_Star2.SetPos(g_cameraPos.x + 500, g_cameraPos.y + -300, 0);
    m_Star3.SetPos(g_cameraPos.x + 800, g_cameraPos.y + -300, 0);

    m_Star1.SetSize(200, 200, 0);
    m_Star2.SetSize(200, 200, 0);
    m_Star3.SetSize(200, 200, 0);

    if (Star != m_prevStar)
    {
        switch (Star) {
        case 0:
            m_Star1.AddTexture("asset/UI/GrayStar.png");
            m_Star2.AddTexture("asset/UI/GrayStar.png");
            m_Star3.AddTexture("asset/UI/GrayStar.png");
            break;
        case 1:
            m_Star1.AddTexture("asset/UI/Star.png");
            m_Star2.AddTexture("asset/UI/GrayStar.png");
            m_Star3.AddTexture("asset/UI/GrayStar.png");
            break;
        case 2:
            m_Star1.AddTexture("asset/UI/Star.png");
            m_Star2.AddTexture("asset/UI/Star.png");
            m_Star3.AddTexture("asset/UI/GrayStar.png");
            break;
        case 3:
            m_Star1.AddTexture("asset/UI/Star.png");
            m_Star2.AddTexture("asset/UI/Star.png");
            m_Star3.AddTexture("asset/UI/Star.png");
            break;
        }
        m_prevStar = Star;
    }

    if (input.GetKeyTrigger(VK_LEFT))
    {
        Star--;
        if (Star < 0) {
            Star = 0;
        }
    }

    if (input.GetKeyTrigger(VK_RIGHT))
    {
        Star++;
        if (Star > 3) {
            Star = 3;
        }
    }


    m_ResultUI.SetPos(g_cameraPos.x, g_cameraPos.y, 0);
    m_ResultUI.SetSize(1920, 1080, 0);

    // スキップ（任意）
    if (input.GetKeyTrigger(VK_RETURN) || input.GetButtonTrigger(XINPUT_A))
    {
        x = 1.0f;
        g_cameraPos = m_resultCam.goal;
        m_resultCam.t = m_resultCam.duration;
    }

    if (x >= 1.0f)
    {
        if (input.GetKeyTrigger(VK_RETURN) || input.GetButtonTrigger(XINPUT_A)) {
            mgr.ChangeScene(SCENE_SELECT);
        }
    }
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

    m_ResultUI.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
    {
        m_Star1.Draw(
            g_pDeviceContext,
            g_pInputLayout,
            g_pVertexShader,
            g_pPixelShader,
            g_pConstantBuffer
        );
        m_Star2.Draw(
            g_pDeviceContext,
            g_pInputLayout,
            g_pVertexShader,
            g_pPixelShader,
            g_pConstantBuffer
        );
        m_Star3.Draw(
            g_pDeviceContext,
            g_pInputLayout,
            g_pVertexShader,
            g_pPixelShader,
            g_pConstantBuffer
        );
    }

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



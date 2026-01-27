#include "SceneSelect.h"
#include "Input.h"

extern Input input;

void SceneSelect::Init() {

    g_cameraPos = { 0, 0, 0 };

    m_select.Init();
    m_select.AddTexture("asset/UI/ステージ選択_背景.png");
    m_select.SetPos(0, 0, 0);
    m_select.SetSize(1920, 1080, 0);

    m_stage1.Init();
    m_stage1.AddTexture("asset/UI/ステージ1_バナー.png");
    m_stage1.SetPos(-400, -250, 0);
    m_stage1.SetSize(700, 500, 0);

    m_stage2.Init();
    m_stage2.AddTexture("asset/UI/ステージ2_バナー.png");
    m_stage2.SetPos(400, -250, 0);
    m_stage2.SetSize(700, 500, 0);

    m_cursor.Init();
    m_cursor.AddTexture("asset/UI/cursor.png");
    m_cursor.SetPos(-650, -50, 0);
    m_cursor.SetSize(200, 200, 0);

    TextureManager& tm = TextureManager::Instance();

    tm.Enqueue("asset/Player/Player_Idle.png");
    tm.Enqueue("asset/Player/Player_Walk.png");
    tm.Enqueue("asset/Player/Player_SmallJump.png");
    tm.Enqueue("asset/Player/Player_Damage.png");
    tm.Enqueue("asset/Player/Player_Ribbon.png");
    tm.Enqueue("asset/Player/Player_RibbonJump.png");
    tm.Enqueue("asset/Player/Player_Pulled.png");
    tm.Enqueue("asset/Player/Player_Roll.png");
    tm.Enqueue("asset/Player/GuideLine.png");
    tm.Enqueue("asset/Player/Circle2.png");
    tm.Enqueue("asset/Player/back.png");
    tm.Enqueue("asset/Player/front.png");

    tm.Enqueue("asset/UI/cursor.png");
    tm.Enqueue("asset/UI/Pouse_Back.png");
    tm.Enqueue("asset/UI/GameOver.png");
    tm.Enqueue("asset/UI/GrayStar.png");
    tm.Enqueue("asset/UI/Star.png");
    tm.Enqueue("asset/UI/Button_Retry.png");
    tm.Enqueue("asset/UI/Button_Continue.png");
    tm.Enqueue("asset/UI/Button_StageSelect.png");
    tm.Enqueue("asset/UI/クリア画面.png");
    tm.Enqueue("asset/UI/ステージ1_バナー.png");
    tm.Enqueue("asset/UI/ステージ2_バナー.png");
    tm.Enqueue("asset/UI/ステージ選択_背景.png");

    tm.Enqueue("asset/Field/aa.png");
    tm.Enqueue("asset/Field/block.png");
    tm.Enqueue("asset/Field/Boad.png");
    tm.Enqueue("asset/Field/Tutorial_PinDeco.png");
    tm.Enqueue("asset/Field/Tutorial_NeedleFloor.png");
    tm.Enqueue("asset/Field/Tutorial_Rippa.png");
    tm.Enqueue("asset/Field/needle_floor.png");
    tm.Enqueue("asset/Field/rippa.png");
    tm.Enqueue("asset/Field/PinJump.png");
    tm.Enqueue("asset/Field/床ブロック.png");
}

void SceneSelect::Update(SceneManager& mgr)
{
    if (Loadtime == 2) {
        TextureManager::Instance().ProcessQueue(g_pDevice, 0.5);
        Loadtime = 0;
    }
        
    Loadtime++;

    DirectX::XMFLOAT2 leftStick = input.GetLeftAnalogStick();// 移動専用
    const float moveThreshold = 0.5f;  //左スティックのデッドゾーン

    if (leftStick.x > moveThreshold)
    {
        m_stickNow = 1;
    }
    else if (leftStick.x < -moveThreshold)
    {
        m_stickNow = 2;
    }
    else
    {
        m_stickNow = 0;
    }

    //左スティック判定
    bool leftStickRTrigger = (m_stickNow ==1) && !m_prevLeftStick;
    bool leftStickLTrigger = (m_stickNow ==2) && !m_prevLeftStick;

    
    int ButtonCount = 0;

    input.Update();
    if (input.GetKeyTrigger(VK_LEFT) ||
        input.GetButtonTrigger(XINPUT_LEFT) ||
        leftStickLTrigger)
    {
        m_stage = 1;
        m_cursor.SetPos(-700, -50, 0);
    }

    if (input.GetKeyTrigger(VK_RIGHT) ||
        input.GetButtonTrigger(XINPUT_RIGHT) ||
        leftStickRTrigger)
    {
        m_stage = 2;
        m_cursor.SetPos(100, -50, 0);
    }


    if (input.GetKeyTrigger(VK_SPACE) || input.GetButtonTrigger(XINPUT_A)) {
        mgr.SetSelectedStage(m_stage);
        mgr.ChangeScene(SCENE_LOAD);
    }

    if (input.GetKeyTrigger(VK_RETURN) || input.GetButtonPress(XINPUT_B)) {
        mgr.SetSelectedStage(99);
        mgr.ChangeScene(SCENE_GAME);
    }

    m_prevLeftStick = m_stickNow;

}


void SceneSelect::Draw() {
    // タイトル描画
    m_select.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
    m_stage1.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
    m_stage2.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
    m_stage3.Draw(
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

void SceneSelect::UnInit() {
    // 解放処理
    m_select.UnInit();
    m_stage1.UnInit();
    m_stage2.UnInit();
    m_stage3.UnInit();
    m_cursor.UnInit();
}
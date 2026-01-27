// SceneTitle.cpp
#include "SceneTitle.h"
#include "Input.h"
extern Input input;

Object m_title;
Object m_select;

void SceneTitle::Init() {
    // タイトル画面初期化
    g_cameraPos = { 0, 0, 0 };

    m_title.Init();
    m_title.AddTexture("asset/UI/タイトル画面_背景.png");
    m_title.SetPos(0, 0, 0);
    m_title.SetSize(1920, 1080, 1);

    m_select.Init();
    m_select.AddTexture("asset/UI/Button_StageSelect.png"); 
    m_select.SetPos(-350, -300, 0);
    m_select.SetSize(490, 150, 1);
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
}

void SceneTitle::Update(SceneManager& mgr) {
    input.Update();

    m_loadDelay += 1/60.0f;
    if (m_loadDelay > 1.0f) { 
        if (Loadtime == 3) {
            TextureManager::Instance().ProcessQueue(g_pDevice, 100);
            Loadtime = 0;
        }
        Loadtime++;
    }

    // スペースキーでゲーム開始
    if (input.GetKeyTrigger(VK_SPACE) || input.GetButtonPress(XINPUT_A)) {
        mgr.ChangeScene(SCENE_SELECT);
    }

    if (input.GetKeyTrigger(VK_RETURN) || input.GetButtonPress(XINPUT_B)) {
        mgr.SetSelectedStage(99);
        mgr.ChangeScene(SCENE_GAME);
    }
}

void SceneTitle::Draw() {
    // タイトル描画
    m_title.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );

    m_select.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
}

void SceneTitle::UnInit() {
    // 解放処理
    m_title.UnInit();
    m_select.UnInit();
}

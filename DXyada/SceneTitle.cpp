// SceneTitle.cpp
#include "SceneTitle.h"
#include "Input.h"
extern Input input;

Object m_title;
void SceneTitle::Init() {
    // タイトル画面初期化
    g_cameraPos = { 0, 0, 0 };

    m_title.Init("asset/title.png");
    m_title.SetPos(0, 0, 0);
    m_title.SetSize(1920, 1080, 1);
}

void SceneTitle::Update(SceneManager& mgr) {
    input.Update();

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
}

void SceneTitle::UnInit() {
    // 解放処理
    m_title.UnInit();
}

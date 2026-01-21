#include "SceneSelect.h"
#include "Input.h"

extern Input input;

void SceneSelect::Init() {

    g_cameraPos = { 0, 0, 0 };

    m_select.Init();
    m_select.AddTexture("asset/Field/back.png");
    m_select.SetPos(0, 0, 0);
    m_select.SetSize(1920, 1080, 0);

    m_stage1.Init();
    m_stage1.AddTexture("asset/UI/stage1.png");
    m_stage1.SetPos(-200, 0, 0);
    m_stage1.SetSize(100, 100, 0);

    m_stage2.Init();
    m_stage2.AddTexture("asset/UI/stage2.png");
    m_stage2.SetPos(0, 0, 0);
    m_stage2.SetSize(100, 100, 0);

    m_stage3.Init();
    m_stage3.AddTexture("asset/UI/stage3.png");
    m_stage3.SetPos(200, 0, 0);
    m_stage3.SetSize(100, 100, 0);

    m_cursor.Init();
    m_cursor.AddTexture("asset/UI/cursor.png");
    m_cursor.SetPos(-240, 40, 0);
    m_cursor.SetSize(50, 50, 0);
}

void SceneSelect::Update(SceneManager& mgr)
{
    m_stage1.SetSize(SizeMin, SizeMin, 0);
    m_stage2.SetSize(SizeMin, SizeMin, 0);
    int ButtonCount = 0;

    input.Update();
    if (input.GetKeyTrigger(VK_LEFT) ||
        input.GetButtonTrigger(XINPUT_LEFT))
    {
        switch (m_stage)
        {
        case 1://ステージ1
            m_stage = 1;
            m_cursor.SetPos(-240, 40, 0);
            break;

        case 2://ステージ1
            m_stage = 1;
            m_cursor.SetPos(-240, 40, 0);
            break;

        case 3://チュートリアルステージ1
            m_stage = 2;
            m_cursor.SetPos(-40, 40, 0);
            break;

        default://デバックステージ
            mgr.SetSelectedStage(99);
        }
    }

    if (input.GetKeyTrigger(VK_RIGHT) ||
        input.GetButtonTrigger(XINPUT_RIGHT))
    {
        switch (m_stage)
        {
        case 1://チュートリアルステージ1
            m_stage = 2;
            m_cursor.SetPos(-40, 40, 0);
            break;

        case 2://チュートリアルステージ1
            m_stage = 3;
            m_cursor.SetPos(160, 40, 0);
            break;

        case 3://チュートリアルステージ2
            m_stage = 3;
            m_cursor.SetPos(160, 40, 0);
            break;

        default://デバックステージ
            mgr.SetSelectedStage(99);
        }
    }

    if (m_stage == 1) m_stage1.SetSize(m_size, m_size, 0);
    if (m_stage == 2) m_stage2.SetSize(m_size, m_size, 0);
    if (m_stage == 3) m_stage3.SetSize(m_size, m_size, 0);


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


    if (input.GetKeyTrigger(VK_SPACE) || input.GetButtonTrigger(XINPUT_A)) {
        mgr.SetSelectedStage(m_stage);
        mgr.ChangeScene(SCENE_GAME);
    }

    if (input.GetKeyTrigger(VK_RETURN) || input.GetButtonPress(XINPUT_B)) {
        mgr.SetSelectedStage(99);
        mgr.ChangeScene(SCENE_GAME);
    }
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
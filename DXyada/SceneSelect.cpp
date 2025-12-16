#include "SceneSelect.h"
#include "Input.h"

extern Input input;

void SceneSelect::Init() {
    
    g_cameraPos = { 0, 0, 0 };

    m_select.Init();
    m_select.AddTexture("asset/back.png");
    m_select.SetPos(0, 0, 0);
    m_select.SetSize(1920, 1080, 0);

    m_stage1.Init();
    m_stage1.AddTexture("asset/stage1.png");
    m_stage1.SetPos(-100, 0, 0);
    m_stage1.SetSize(100, 100, 0);

    m_stage2.Init();
    m_stage2.AddTexture("asset/stage2.png");
    m_stage2.SetPos(100, 0, 0);
    m_stage2.SetSize(100, 100, 0);

    m_cursor.Init();
    m_cursor.AddTexture("asset/cursor.png");
    m_cursor.SetPos(-140, 40, 0);
    m_cursor.SetSize(50, 50, 0);
}

void SceneSelect::Update(SceneManager& mgr)
{
    m_stage1.SetSize(SizeMin, SizeMin, 0);
    m_stage2.SetSize(SizeMin, SizeMin, 0);
    
        input.Update();
    if (input.GetKeyPress(VK_LEFT)) {
        m_stage = 1;
        m_cursor.SetPos(-140, 40, 0);
    }
    if (input.GetKeyPress(VK_RIGHT)) {
        m_stage = 2;
        m_cursor.SetPos(60, 40, 0);
    }

    if (m_stage == 1) m_stage1.SetSize(m_size, m_size, 0);
    if (m_stage == 2) m_stage2.SetSize(m_size, m_size, 0);
        

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


    if (input.GetKeyTrigger(VK_RETURN)) {
        mgr.SetSelectedStage(m_stage);
        mgr.ChangeScene(SCENE_GAME);
    }
}


void SceneSelect::Draw() {
    // É^ÉCÉgÉãï`âÊ
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
    m_cursor.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
}

void SceneSelect::UnInit() {
    // âï˙èàóù
    m_select.UnInit();
    m_stage1.UnInit();
    m_stage2.UnInit();
    m_cursor.UnInit();
}
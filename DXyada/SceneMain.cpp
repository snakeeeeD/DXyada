#include "SceneMain.h"
#include "SceneManager.h"
#include "Input.h"
#include "Renderer.h"
#include "Platform.h"

extern Input input;

Platform platforms;

void SceneMain::Init() {
    m_background.Init("asset/aa.png");
    m_background.SetPos(0, 0, 0);
    m_background.SetSize(1920, 1080, 0);

    m_block.Init("asset/block.png", 0, -100, 100, 50);
    m_block1.Init("asset/block.png", -300, -400, 400, 80);



    m_player.Init();



    m_collision.AddDynamic(m_player.GetObject());
    m_collision.AddStatic(m_block.GetObject());
    m_collision.AddStatic(m_block1.GetObject());

    

}

void SceneMain::Update(SceneManager& mgr) {

    if (input.GetKeyTrigger(VK_ESCAPE)) {
        mgr.ChangeScene(SCENE_TITLE);
    }
   // m_player.Update(1/240.0f);

    m_collision.CheckAll();
  
}

void SceneMain::Draw() {
    m_background.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );

    m_block.Draw();
    m_block1.Draw();

    m_player.Draw();

}

void SceneMain::UnInit() {
    m_background.UnInit();
    m_player.Uninit();
}


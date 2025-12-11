#include "Stage2.h"
#include "SceneGame.h"

void Stage2::Init() {
    g_StartPlayer = { 150,-50,0 };

    //îwåi
    m_background.Init("asset/aa.png");
    m_background.SetPos(0, 0, 0);
    m_background.SetSize(10000, 9000, 0);

    // ë´èÍ1
    Platform plat1;
    plat1.Init("asset/block.png", 0, -200, 100, 50);
    m_platforms.push_back(plat1);

    // ë´èÍ2
    Platform plat2;
    plat2.Init("asset/block.png", 150, -150, 100, 50);
    m_platforms.push_back(plat2);
    // ë´èÍÇR
    Platform plat3;
    plat3.Init("asset/block.png", 1000, -150, 1800, 50);
    m_platforms.push_back(plat3);

    Platform plat4;
    plat4.Init("asset/block.png", 1000, 350, 1800, 50);
    m_platforms.push_back(plat4);


    enemy1.Init("asset/title.png", 300, -75, 100, 100);
    m_enemy.push_back(enemy1);

}

void Stage2::Update() {
    for (auto& enemy : m_enemy) {
        enemy.Update(1.0 / 240.0f);
    }
}

void Stage2::Draw() {

    /*m_background.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer);*/

    for (auto& plat : m_platforms) {
        plat.Draw();
    }
    for (auto& enemy : m_enemy) {
        enemy.Draw();
    }
}

void Stage2::UnInit() {
    for (auto& plat : m_platforms) {
        plat.UnInit();
    }
    for (auto& enemy : m_enemy) {
        enemy.UnInit();
    }
    m_platforms.clear();

    m_background.UnInit();
}
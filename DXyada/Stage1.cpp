#include "Stage1.h"
#include "SceneGame.h"

void Stage1::Init() {
   //Playerの初期位置設定　グローバルに設定してるのでヘッダ側でPlayer.hを継承すれば即使えるようなる
   //実は仕組み的にカメラとなんも変わんない
    g_StartPlayer = { 150,-50,0 };

    //背景
    m_background.Init("asset/aa.png");
    m_background.SetPos(0, 0, 0);
    m_background.SetSize(19200, 10800, 0);

    // 足場1
    Platform plat1;
    plat1.Init("asset/block.png", 0, -200, 100, 50);
    m_platforms.push_back(plat1);

    // 足場2
    Platform plat2;
    plat2.Init("asset/block.png", 150, -150, 100, 50);
    m_platforms.push_back(plat2);
    // 足場３
    Platform plat3;
    plat3.Init("asset/block.png", 1000, -150, 1800, 50);
    m_platforms.push_back(plat3);


    enemy1.Init("asset/title.png", -100, -175, 100, 100);
    m_enemy.push_back(enemy1);

}

void Stage1::Update() {
    for (auto& enemy : m_enemy) {
        enemy.Update(1.0/240.0f);
    }
}

void Stage1::Draw() {
    
    m_background.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer);

    for (auto& plat : m_platforms) {
        plat.Draw();
    }
    for (auto& enemy : m_enemy) {
        enemy.Draw();
    }
}

void Stage1::UnInit() {
    for (auto& plat : m_platforms) {
        plat.UnInit();
    }
    for (auto& enemy : m_enemy) {
        enemy.UnInit();
    }
    m_platforms.clear();

    m_background.UnInit();
}

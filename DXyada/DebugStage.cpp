#include "DebugStage.h"
#include "Renderer.h"
#include <algorithm>

void DebugStage::Init()
{

    g_cameraPos = { 0, 0, 0 };

    m_collision = new CollisionManager();

    // プレイヤー初期位置
    m_player.Init();
    m_player.GetObject()->SetPos(0, -200, 0);

    // 背景
    m_background.Init("asset/aa.png");
    m_background.SetPos(0, 0, 0);
    m_background.SetSize(19200, 10800, 0);


    // 足場
    Platform p1; p1.Init("asset/block.png", 0, -600, 1800, 600);

    m_platforms = { p1 };

    for (auto& plat : m_platforms) {
        m_collision->AddStatic(plat.GetObject());
        m_collision->SetTag(plat.GetObject(), ColliderTag::Platform);
    }

    for (auto& enemy : m_enemies) {
        m_collision->AddStatic(enemy.GetObject());
        m_collision->SetTag(enemy.GetObject(), ColliderTag::Enemy);
    }

    // プレイヤーのコリジョン登録
    m_collision->AddDynamic(m_player.GetObject());
    m_collision->SetTag(m_player.GetObject(), ColliderTag::Player);

    ///////////////////////////////////////////////////////////びょうがじゅん～//////////////////////////////////////////////////////////

    DrawItem item;

    // 背景
    item.obj = &m_background;
    item.layer = DrawLayer::Background;
    m_drawList.push_back(item);

    // 足場（ステージオブジェクト）
    for (size_t i = 0; i < m_platforms.size(); ++i) {
        item.obj = m_platforms[i].GetObject();
        item.layer = DrawLayer::StageObject;
        m_drawList.push_back(item);
    }

    // 敵
    for (size_t i = 0; i < m_enemies.size(); ++i) {
        item.obj = m_enemies[i].GetObject();
        item.layer = DrawLayer::Enemy;
        m_drawList.push_back(item);
    }

    // プレイヤー
    item.obj = m_player.GetObject();
    item.layer = DrawLayer::Player;
    m_drawList.push_back(item);
}

void DebugStage::Update()
{
    float dt = 1.0f / 60.0f;

    // プレイヤー更新
    m_player.Update(dt, m_platforms, m_enemies);

    // 敵更新
    for (auto& enemy : m_enemies) {
        enemy.Update(dt);
    }

    // カメラ追従
 //   m_camera.Update(m_player.GetObject()->GetPos());

    // 衝突判定
    m_collision->CheckAll();

    // 死亡判定（プレイヤーのHPで判定）
    //if (m_player.GetHP() <= 0) {
    m_isPlayerDead = true;
    //}
}

void DebugStage::Draw()
{
    std::sort(
        m_drawList.begin(),
        m_drawList.end(),
        [](const DrawItem& a, const DrawItem& b) {
            return static_cast<int>(a.layer) < static_cast<int>(b.layer);
        }
    );

    // 並べ替えた順に描画
    for (size_t i = 0; i < m_drawList.size(); ++i) {
        Object* obj = m_drawList[i].obj;
        if (!obj) continue; // 念のため

        obj->Draw(
            g_pDeviceContext,
            g_pInputLayout,
            g_pVertexShader,
            g_pPixelShader,
            g_pConstantBuffer
        );
    }
}

void DebugStage::UnInit()
{
    for (auto& plat : m_platforms) plat.UnInit();
    for (auto& enemy : m_enemies) enemy.UnInit();

    m_background.UnInit();
    m_player.Uninit();

    delete m_collision;
    m_collision = nullptr;
}

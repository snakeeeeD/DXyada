#include "Stage2.h"
#include "Renderer.h"
#include <algorithm>

void Stage2::Init()
{
    m_collision = new CollisionManager();

    // プレイヤー初期位置
    m_player.Init();
    m_player.SetCollisionManager(m_collision);
    m_player.GetObject()->SetPos(150, -50, 0);

    // 背景
    m_background.Init();
    m_background.AddTexture("asset/Field/aa.png");
    m_background.SetPos(0, 0, 0);
    m_background.SetSize(19200, 10800, 0);


    // 足場
    Platform* p1 = new Platform(); p1->Init("asset/Field/block.png", 0, -200, 100, 50);
    Platform* p2 = new Platform(); p2->Init("asset/Field/block.png", 150, -150, 100, 50);
    Platform* p3 = new Platform(); p3->Init("asset/Field/block.png", 1000, -150, 1800, 50);
    Platform* p4 = new Platform(); p4->Init("asset/Field/block.png", 1000, 350, 1800, 50);

    //m_platforms = { p1, p2, p3, p4 };
    m_platforms.push_back(p1);
    m_platforms.push_back(p2);
    m_platforms.push_back(p3);
    m_platforms.push_back(p4);

    for (auto& plat : m_platforms) {
        m_collision->AddStatic(plat->GetObject());
        m_collision->SetTag(plat->GetObject(), ColliderTag::Platform);
    }

    // 敵
    /*Enemy e;
    e.Init("asset/title.png", 300, -75, 100, 100);
    m_enemies.push_back(e);*/

    for (auto& enemy : m_enemies) {
        m_collision->AddMoved(enemy->GetObject());
        m_collision->SetTag(enemy->GetObject(), ColliderTag::Enemy);
    }

    m_HP_UI1.Init();
    m_HP_UI1.AddTexture("asset/UI/cursor.png");
    m_HP_UI1.SetSize(200, 200, 0);

    m_HP_UI2.Init();
    m_HP_UI2.AddTexture("asset/UI/cursor.png");
    m_HP_UI2.SetSize(200, 200, 0);

    m_HP_UI3.Init();
    m_HP_UI3.AddTexture("asset/UI/cursor.png");
    m_HP_UI3.SetSize(200, 200, 0);

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
        item.obj = m_platforms[i]->GetObject();
        item.layer = DrawLayer::StageObject;
        m_drawList.push_back(item);
    }

    // 敵
    for (size_t i = 0; i < m_enemies.size(); ++i) {
        item.obj = m_enemies[i]->GetObject();
        item.layer = DrawLayer::Enemy;
        m_drawList.push_back(item);
    }

    // プレイヤー
    item.obj = m_player.GetObject();
    item.layer = DrawLayer::Player;
    m_drawList.push_back(item);

    item.obj = m_player.GetGuideLineObject();
    item.layer = DrawLayer::BackObject;
    m_drawList.push_back(item);



    // リボン描画登録
    for (auto* obj : m_player.GetRibbon().GetDrawObjects())
    {
        DrawItem item;
        item.obj = obj;
        item.layer = DrawLayer::BackObject;
        m_drawList.push_back(item);
    }

    item.obj = &m_HP_UI1;
    item.layer = DrawLayer::UI;
    m_drawList.push_back(item);

    item.obj = &m_HP_UI2;
    item.layer = DrawLayer::UI;
    m_drawList.push_back(item);

    item.obj = &m_HP_UI3;
    item.layer = DrawLayer::UI;
    m_drawList.push_back(item);
}

void Stage2::Update()
{
    float dt = 1.0f / 60.0f;

    // プレイヤー更新
    m_player.Update(dt, m_platforms, m_enemies, m_pins);

    // 敵更新
    for (auto& enemy : m_enemies) 
    {
        enemy->Update(dt);
    }

    // カメラ追従
    m_camera.Update(m_player.GetObject()->GetPos());
    m_HP_UI1.SetPos(g_cameraPos.x - 800, g_cameraPos.y + 400, 0);
    m_HP_UI2.SetPos(g_cameraPos.x - 600, g_cameraPos.y + 400, 0);
    m_HP_UI3.SetPos(g_cameraPos.x - 400, g_cameraPos.y + 400, 0);

    currentHP = m_player.GetHP();

    switch (currentHP)
    {
    case 3:
        m_HP_UI1.SetColor(1.0, 1.0, 1.0, 1.0);
        m_HP_UI2.SetColor(1.0, 1.0, 1.0, 1.0);
        m_HP_UI3.SetColor(1.0, 1.0, 1.0, 1.0);
        break;
    case 2:
        m_HP_UI1.SetColor(1.0, 1.0, 1.0, 1.0);
        m_HP_UI2.SetColor(1.0, 1.0, 1.0, 1.0);
        m_HP_UI3.SetColor(0.1, 0.1, 0.1, 1.0);
        break;
    case 1:
        m_HP_UI1.SetColor(1.0, 1.0, 1.0, 1.0);
        m_HP_UI2.SetColor(0.1, 0.1, 0.1, 1.0);
        m_HP_UI3.SetColor(0.1, 0.1, 0.1, 1.0);
        break;
    }

    for (auto& enemy : m_enemies) {
        enemy->Update(dt);

        //プレイヤーと敵との衝突判定
        auto playerAABB = m_collision->GetAABB(m_player.GetObject());
        auto enemyAABB = m_collision->GetAABB(enemy->GetObject());

        if (m_collision->CheckOverlap(playerAABB, enemyAABB))
        {
            // ノックバック方向を計算（プレイヤー位置 - 敵位置）
            DirectX::XMFLOAT3 playerPos = m_player.GetObject()->GetPos();
            DirectX::XMFLOAT3 enemyPos = enemy->GetObject()->GetPos();

            DirectX::XMFLOAT2 knockbackDir = {
                playerPos.x - enemyPos.x,
                playerPos.y - enemyPos.y
            };

            m_player.TakeDamage(1, knockbackDir);
        }
    }

    // 衝突判定
    m_collision->CheckAll();

    // 死亡判定（プレイヤーのHPで判定）
    if (m_player.isDead()) {
        m_isPlayerDead = true;
    }
}

void Stage2::Draw()
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

void Stage2::UnInit()
{
    for (auto& plat : m_platforms) plat->UnInit();
    for (auto& enemy : m_enemies) enemy->UnInit();

    m_background.UnInit();
    m_player.Uninit();

    delete m_collision;
    m_collision = nullptr;
}

#include "Stage1.h"
#include "SceneGame.h"
#include <algorithm>
void Stage1::Init() {
    m_collision = new CollisionManager();

    m_background.Init("asset/aa.png");
    m_background.SetPos(0, 0, 0);
    m_background.SetSize(19200, 10800, 0);

    m_usiro.Init("asset/block.png");
    m_usiro.SetPos(100, 0, 0);
    m_usiro.SetSize(100, 100, 0);

    m_mae.Init("asset/block.png");
    m_mae.SetPos(0, 0, 0);
    m_mae.SetSize(100, 100, 0);
    
    m_player.Init();
    m_player.GetObject()->SetPos(150, -50, 0);

    Platform plat1; plat1.Init("asset/block.png", 0, -200, 100, 50);
    Platform plat2; plat2.Init("asset/block.png", 150, -150, 100, 50);
    Platform plat3; plat3.Init("asset/block.png", 1000, -150, 1800, 50);

    Enemy e;
    e.Init("asset/title.png", 300, -50, 100, 100);
    m_enemies.push_back(e);

    m_platforms = { plat1, plat2, plat3 };
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

    //////////////////////////////////////////////////////////////////描画設定のやり方//////////////////////////////////////////////////////////////////////
    DrawItem item;

    item.obj = &m_background;
    item.layer = DrawLayer::Background;
    m_drawList.push_back(item);

    item.obj = &m_usiro;
    item.layer = DrawLayer::BackObject;
    m_drawList.push_back(item);

    for (size_t i = 0; i < m_platforms.size(); ++i) {
        item.obj = m_platforms[i].GetObject();
        item.layer = DrawLayer::StageObject;
        m_drawList.push_back(item);
    }

    for (size_t i = 0; i < m_enemies.size(); ++i) {
        item.obj = m_enemies[i].GetObject();
        item.layer = DrawLayer::Enemy;
        m_drawList.push_back(item);
    }

    item.obj = m_player.GetObject();
    item.layer = DrawLayer::Player;
    m_drawList.push_back(item);

    item.obj = m_player.GetGuideLineObject();
    item.layer = DrawLayer::BackObject;
    m_drawList.push_back(item);

    item.obj = &m_mae;
    item.layer = DrawLayer::FrontObject;
    m_drawList.push_back(item);
}

void Stage1::Update() {
    float dt = 1.0f / 60.0f;

    // プレイヤー更新（Stage 内で完結）
    m_player.Update(dt, m_platforms, m_enemies);

    // 敵更新
    for (auto& enemy : m_enemies) {
        enemy.Update(dt);

        //プレイヤーと敵との衝突判定
        auto playerAABB = m_collision->GetAABB(m_player.GetObject());
        auto enemyAABB = m_collision->GetAABB(enemy.GetObject());

        if (m_collision->CheckOverlap(playerAABB, enemyAABB))
        {
            // ノックバック方向を計算（プレイヤー位置 - 敵位置）
            DirectX::XMFLOAT3 playerPos = m_player.GetObject()->GetPos();
            DirectX::XMFLOAT3 enemyPos = enemy.GetObject()->GetPos();

            DirectX::XMFLOAT2 knockbackDir = {
                playerPos.x - enemyPos.x,
                playerPos.y - enemyPos.y
            };

            m_player.TakeDamage(1, knockbackDir);
        }
    }

    // カメラ更新
    m_camera.Update(m_player.GetObject()->GetPos());

    // コリジョン更新
    m_collision->CheckAll();

    // 死亡判定（あなたの処理に合わせて書き換え可能）
    if (m_player.isDead()) {  // ← この条件を追加
        m_isPlayerDead = true;
    }
}

void Stage1::Draw() {
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

void Stage1::UnInit() {
    for (auto& plat : m_platforms) plat.UnInit();
    for (auto& enemy : m_enemies) enemy.UnInit();

    m_background.UnInit();
    m_player.Uninit();

    delete m_collision;
}

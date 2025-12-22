#include "Stage1.h"
#include "SceneGame.h"
#include "Enemy.h"
#include "Rippa.h"
#include "NeedleFloor.h"
#include <algorithm>
void Stage1::Init() 
{
    m_collision = new CollisionManager();

    m_background.Init();
    m_background.AddTexture("asset/aa.png");
    m_background.SetPos(0, 0, 0);
    m_background.SetSize(19200, 10800, 0);

    m_usiro.Init();
    m_usiro.AddTexture("asset/block.png");
    m_usiro.SetPos(100, 0, 0);
    m_usiro.SetSize(100, 100, 0);

    m_mae.Init();
    m_mae.AddTexture("asset/block.png");
    m_mae.SetPos(0, 0, 0);
    m_mae.SetSize(100, 100, 0);
    
    m_player.Init();
    m_player.SetCollisionManager(m_collision);
    m_player.GetObject()->SetPos(150, -50, 0);

    Platform plat1; plat1.Init("asset/block.png", 0, -200, 100, 50);     //手前床
    Platform plat2; plat2.Init("asset/block.png", 150, -500, 100, 50);   //主人公が乗っている床
    Platform plat3; plat3.Init("asset/block.png", 1000, -150, 1500, 50); //ナガ床

    Platform plat4; plat4.Init("asset/block.png", 400, -100, 100, 50); //左壁
    Platform plat5; plat5.Init("asset/block.png", 1000, -100, 100, 50);  //右壁

    Platform plat6; plat6.Init("asset/block.png", 1000, 100, 500, 50);  //リッパー崖判定の床

    Platform plat7; plat7.Init("asset/block.png", 1000, -500, 1500, 50);  //下ナガ床

    /*Enemy e;
    e.Init("asset/title.png", 1000, -50, 100, 100);
    m_enemies.push_back(e);*/

  /*Enemy Rippa;
    Rippa.Init("asset/rippa.png", 700, 300, 100, 100);
    m_enemies.push_back(Rippa);*/

    //エネミー1 (壁反転)
    {
        Rippa* rippa = new Rippa(Rippa::Type::Normal);
        rippa->Init("asset/rippa.png", 800, -100, 100, 100);
        rippa->SetCollisionManager(m_collision);
        m_enemies.push_back(rippa);
    }

    //エネミー2 (崖反転)
    {
        Rippa* rippa_2 = new Rippa(Rippa::Type::Normal);
        rippa_2->Init("asset/rippa.png", 1000, 150, 100, 100);
        rippa_2->SetCollisionManager(m_collision);

        m_enemies.push_back(rippa_2);
    }

    //エネミー3 (徘徊)
    {
        Rippa* rippa_3 = new Rippa(Rippa::Type::Wandering);
        rippa_3->Init("asset/rippa.png", 1000, -450, 100, 100);
        rippa_3->SetCollisionManager(m_collision);

        rippa_3->SetTurnInterval(2.5f); //個々の値変更で自由に変えられる

        m_enemies.push_back(rippa_3);
    }


    //ハリ床
    {
        NeedleFloor* nf = new NeedleFloor();

        // 床の高さに合わせて y=-150 に配置
        nf->Init("asset/needle_floor.png", 0, -300, 150, 50);

        // 忘れずに CollisionManager をセット（将来のリボン判定用）
        nf->SetCollisionManager(m_collision);

        m_enemies.push_back(nf);
    }

    m_platforms = { plat1, plat2, plat3, plat4, plat5, plat6, plat7 };
    for (auto& plat : m_platforms) 
    {
        m_collision->AddStatic(plat.GetObject());
        m_collision->SetTag(plat.GetObject(), ColliderTag::Platform);
    }

    for (auto& enemy : m_enemies) 
    {
        m_collision->AddMoved(enemy->GetObject());
        m_collision->SetTag(enemy->GetObject(), ColliderTag::Enemy);
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

    for (size_t i = 0; i < m_platforms.size(); ++i) 
    {
        item.obj = m_platforms[i].GetObject();
        item.layer = DrawLayer::StageObject;
        m_drawList.push_back(item);
    }

    for (size_t i = 0; i < m_enemies.size(); ++i) 
    {
        item.obj = m_enemies[i]->GetObject();
        item.layer = DrawLayer::Enemy;
        m_drawList.push_back(item);
    }

    item.obj = m_player.GetObject();
    item.layer = DrawLayer::Player;
    m_drawList.push_back(item);

    item.obj = m_player.GetGuideLineObject();
    item.layer = DrawLayer::BackObject;
    m_drawList.push_back(item);

    item.obj = m_player.GetCircle();
    item.layer = DrawLayer::BackObject;
    m_drawList.push_back(item);

    item.obj = &m_mae;
    item.layer = DrawLayer::FrontObject;
    m_drawList.push_back(item);


    // リボン描画登録
    for (auto* obj : m_player.GetRibbon().GetDrawObjects())
    {
        DrawItem item;
        item.obj = obj;
        item.layer = DrawLayer::BackObject;
        m_drawList.push_back(item);
    }

}

void Stage1::Update() 
{
    float dt = 1.0f / 60.0f;

    // プレイヤー更新（Stage 内で完結）
    m_player.Update(dt, m_platforms, m_enemies);

    // 敵更新
    for (auto& enemy : m_enemies) 
    {

        //範囲判定
        float distance = 0.0f;
        bool inRange = m_player.IsEnemyInRange(enemy->GetObject()->GetPos(), distance);
        enemy->SetHighlight(inRange);

        enemy->Update(dt);

        //プレイヤーと敵との衝突判定
        auto playerAABB = m_collision->GetAABB(m_player.GetObject());
        auto enemyAABB = m_collision->GetAABB(enemy->GetObject());

        if (m_collision->CheckOverlap(playerAABB, enemyAABB))
        {
            // ノックバック方向を計算（プレイヤー位置 - 敵位置）
            DirectX::XMFLOAT3 playerPos = m_player.GetObject()->GetPos();
            DirectX::XMFLOAT3 enemyPos = enemy->GetObject()->GetPos();

            DirectX::XMFLOAT2 knockbackDir = 
            {
                playerPos.x - enemyPos.x,
                playerPos.y - enemyPos.y
            };

            m_player.TakeDamage(1, knockbackDir);
        }
    }

    for (auto* enemy : m_enemies)
    {
        enemy->Update(dt);
    }

    m_enemies.erase(
        std::remove_if(
            m_enemies.begin(),
            m_enemies.end(),
            [&](Enemy* enemy)
            {
                if (enemy->IsDead())
                {
                    m_collision->Remove(enemy->GetObject());
                    return true;
                }
                return false;
            }
        ),
        m_enemies.end()
    );


    // カメラ更新
    m_camera.Update(m_player.GetObject()->GetPos());

    // コリジョン更新
    m_collision->CheckAll();

    if (m_player.isDead()) 
    {  
        m_isPlayerDead = true;
    }
}

void Stage1::Draw() 
{
    std::sort(
        m_drawList.begin(),
        m_drawList.end(),
        [](const DrawItem& a, const DrawItem& b) 
        {
            return static_cast<int>(a.layer) < static_cast<int>(b.layer);
        }
    );

    // 並べ替えた順に描画
    for (size_t i = 0; i < m_drawList.size(); ++i) 
    {
        Object* obj = m_drawList[i].obj;
        if (!obj) continue; // 念のため

        obj->Draw
        (
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
    for (auto& enemy : m_enemies) enemy->UnInit();

    m_background.UnInit();
    m_player.Uninit();

    delete m_collision;
    m_collision = nullptr;
}

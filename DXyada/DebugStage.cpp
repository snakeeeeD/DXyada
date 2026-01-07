#include "DebugStage.h"
#include "Renderer.h"
#include <algorithm>

void DebugStage::Init()
{

    g_cameraPos = { 0, 0, 0 };

    m_collision = new CollisionManager();

    // プレイヤー初期位置
    m_player.Init();
    m_player.SetCollisionManager(m_collision);
    m_player.GetObject()->SetPos(0, -200, 0);

    // 背景
    m_background.Init();
    m_background.AddTexture("asset/Field/aa.png");
    m_background.SetPos(0, 0, 0);
    m_background.SetSize(19200, 10800, 0);


    // 足場
    Platform p1; p1.Init("asset/Field/block.png", 0, -600, 1800, 600);

    m_platforms = { p1 };

    Enemy* e = new Enemy();
    e->Init("asset/Field/rippa.png", 300, -300, 100, 100);
    m_enemies.push_back(e);


    for (auto& plat : m_platforms) {
        m_collision->AddStatic(plat.GetObject());
        m_collision->SetTag(plat.GetObject(), ColliderTag::Platform);
    }

    for (auto& enemy : m_enemies) {
        m_collision->AddStatic(enemy->GetObject());
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
        item.obj = m_platforms[i].GetObject();
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

    item.obj = m_player.GetCircle();
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

    //ピン
    {
        Pin* pin = new Pin();
        pin->Init("asset/Field/Pin.png", -450, 0, 35, 35);
        pin->SetCollisionManager(m_collision);
        m_pins.push_back(pin);
        pin->SetcanRollPin(false);
    }
    //ピン付きブロック (地面付近のやつ)
    {
        m_BlockPinPos.x = -300.0f;
        m_BlockPinPos.y = -250.0f;

        BlockPin* blockpin = new BlockPin();
        blockpin->Init("asset/Field/Pin.png", m_BlockPinPos.x, m_BlockPinPos.y, 35, 35); //画像でき次第パス名を変更
        blockpin->SetCollisionManager(m_collision);
        m_pins.push_back(blockpin);
        blockpin->SetcanRollPin(true);

        // BlockPinのコリジョンを登録
        m_collision->AddStatic(blockpin->GetObject());
        m_collision->SetTag(blockpin->GetObject(), ColliderTag::Pin);
    }

    {
        m_BlockPinPos.x = -600.0f;
        m_BlockPinPos.y = -250.0f;

        BlockPin* blockpin2 = new BlockPin();
        blockpin2->Init("asset/Field/Pin.png", m_BlockPinPos.x, m_BlockPinPos.y, 35, 35); //画像でき次第パス名を変更
        blockpin2->SetCollisionManager(m_collision);
        m_pins.push_back(blockpin2);
        blockpin2->SetcanRollPin(false);

        // BlockPinのコリジョンを登録
        m_collision->AddStatic(blockpin2->GetObject());
        m_collision->SetTag(blockpin2->GetObject(), ColliderTag::Pin);
    }

}

void DebugStage::Update()
{
    float dt = 1.0f / 60.0f;

    // プレイヤー更新
    m_player.Update(dt, m_platforms, m_enemies, m_pins);

    // 敵更新 
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


    for (auto* pin : m_pins)
    {
        pin->Update(dt);

        // BlockPinの場合、移動後のコリジョン更新
        BlockPin* blockPin = dynamic_cast<BlockPin*>(pin);
        if (blockPin)
        {

            m_collision->GetAABB(blockPin->GetObject());
        }
    }

    // 衝突判定
    m_collision->CheckAll();

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

    // 死亡判定（プレイヤーのHPで判定）
        // 死亡判定（プレイヤーのHPで判定）
    if (m_player.isDead()) {
        m_isPlayerDead = true;
    }
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

    for (auto* pin : m_pins)
    {
        pin->Draw();
    }
}

void DebugStage::UnInit()
{
    for (auto& plat : m_platforms) plat.UnInit();
    for (auto& enemy : m_enemies)
    {
        enemy->UnInit();
        delete enemy;
    }
    m_enemies.clear();

    m_background.UnInit();
    m_player.Uninit();
    for (auto& pin : m_pins) pin->UnInit();

    delete m_collision;
    m_collision = nullptr;
}

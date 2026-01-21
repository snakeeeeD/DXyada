#include "TutorialStage.h"
#include "Renderer.h"
#include <algorithm>

// 既存と同様に外部にある想定
extern DirectX::XMFLOAT3 g_cameraPos;

void TutorialStage::AddPlatform(const char* tex, float x, float y, float w, float h)
{
    Platform* p = new Platform();
    p->Init(tex, x, y, w, h);
    m_platforms.push_back(p);

    Platform* back = m_platforms.back();
    m_collision->AddStatic(back->GetObject());
    m_collision->SetTag(back->GetObject(), ColliderTag::Platform);
}

void TutorialStage::AddDecorPin(float x, float y, bool canDecorate)
{
    Pin* pin = new Pin();
    pin->Init("asset/Field/Pin.png", x, y, 35, 35);
    pin->SetCollisionManager(m_collision);

    pin->SetcanRollPin(true);
    pin->SetcanDecorate(canDecorate);

    m_pins.push_back(pin);
}

BlockPin* TutorialStage::AddPullPin(float x, float y, bool canRollPin)
{
    BlockPin* pin = new BlockPin();
    pin->Init("asset/Field/Pin.png", x, y, 35, 35);
    pin->SetCollisionManager(m_collision);

    pin->SetcanRollPin(canRollPin);
    pin->SetcanDecorate(false);

    m_pins.push_back(pin);

    // 掴む用のタグは Pin のまま推奨
    m_collision->SetTag(pin->GetObject(), ColliderTag::Pin);

    return pin;
}

//void TutorialStage::AddPullPin(float x, float y, bool canRollPin)
//{
//    BlockPin* blockpin = new BlockPin();
//    blockpin->Init("asset/Field/Pin.png", x, y, 35, 35);
//    blockpin->SetCollisionManager(m_collision);
//
//    blockpin->SetcanRollPin(canRollPin);
//    blockpin->SetcanDecorate(false);
//
//    m_pins.push_back(blockpin);
//
//    m_collision->SetTag(blockpin->GetObject(), ColliderTag::Pin);
//}


void TutorialStage::BuildDrawList()
{
    m_drawList.clear();

    DrawItem item;

    item.obj = &m_background;
    item.layer = DrawLayer::Background;
    m_drawList.push_back(item);

    for (size_t i = 0; i < m_platforms.size(); ++i) {
        item.obj = m_platforms[i]->GetObject();
        item.layer = DrawLayer::StageObject;
        m_drawList.push_back(item);
    }

    for (size_t i = 0; i < m_enemies.size(); ++i) {
        item.obj = m_enemies[i]->GetObject();
        item.layer = DrawLayer::Enemy;
        m_drawList.push_back(item);
    }

    for (size_t i = 0; i < m_tutorials.size(); ++i) {
        item.obj = m_tutorials[i]->GetObject();
        item.layer = DrawLayer::StageObject;
        m_drawList.push_back(item);
    }

    for (size_t i = 0; i < m_tutorials.size(); ++i) {
        item.obj = m_tutorials[i]->GetDisplayObject();
        item.layer = DrawLayer::StageObject;
        m_drawList.push_back(item);
    }

    item.obj = m_player.GetObject();
    item.layer = DrawLayer::Player;
    m_drawList.push_back(item);

    item.obj = m_player.GetGuideLineObject();
    item.layer = DrawLayer::Player;
    m_drawList.push_back(item);

    item.obj = m_player.GetCircle();
    item.layer = DrawLayer::Player;
    m_drawList.push_back(item);

    for (auto* obj : m_player.GetRibbon().GetDrawObjects())
    {
        DrawItem it;
        it.obj = obj;
        it.layer = DrawLayer::Player;
        m_drawList.push_back(it);
    }

  

    item.obj = &m_HP_UI1; item.layer = DrawLayer::UI; m_drawList.push_back(item);
    item.obj = &m_HP_UI2; item.layer = DrawLayer::UI; m_drawList.push_back(item);
    item.obj = &m_HP_UI3; item.layer = DrawLayer::UI; m_drawList.push_back(item);
}

void TutorialStage::Init()
{
    g_cameraPos = { 0, 0, 0 };

    m_collision = new CollisionManager();

    m_player.Init();
    m_player.SetCollisionManager(m_collision);
    m_player.GetObject()->SetPos(0, 150, 0);

    m_currentCheckpoint = { 0, 150, 0 };
    m_hasCheckpoint = true;

    m_background.Init();
    m_background.AddTexture("asset/Field/aa.png");
    m_background.SetPos(0, 0, 0);
    m_background.SetSize(192000, 108000, 0);

    m_HP_UI1.Init(); m_HP_UI1.AddTexture("asset/UI/cursor.png"); m_HP_UI1.SetSize(200, 200, 0);
    m_HP_UI2.Init(); m_HP_UI2.AddTexture("asset/UI/cursor.png"); m_HP_UI2.SetSize(200, 200, 0);
    m_HP_UI3.Init(); m_HP_UI3.AddTexture("asset/UI/cursor.png"); m_HP_UI3.SetSize(200, 200, 0);

    m_collision->AddDynamic(m_player.GetObject());
    m_collision->SetTag(m_player.GetObject(), ColliderTag::Player);

    const float TILE = 150.0f;
    const float H = 600.0f;

    const float LOW_Y = -600.0f;
    const float HIGH_Y = LOW_Y + TILE;

    float x = -300.0f;

    m_platforms.clear();
    m_platforms.reserve(32);

    //1
    {
        float w = TILE * 10.0f;
        AddPlatform("asset/Field/block.png", x + w * 0.5f, LOW_Y, w, H);

        AddDecorPin(x + TILE * 10.0f+ 0.85f * TILE, -317.0f, true);

        Tutorial* tutorial1 = new Tutorial();
        tutorial1->Init(
            "asset/Field/Boad.png",
            1000, -230, 200, 200,
            Tutorial::Type::Move,
            true,   //チュートリアル表示ON
            true    //チェックポイントON
        );
        tutorial1->InitTutorialImage(
            "asset/Ui/title.png",
            1000, 750,   //表示サイズ
            300, 450      //看板からのオフセット
        );
        tutorial1->SetRespawnPosition(100, -150, 0);
        m_tutorials.push_back(tutorial1);

        x += w;

        x += TILE * 1.7f;

    }

    //2
    {
        float w1 = TILE * 8.0f;
        AddPlatform("asset/Field/block.png", x + w1 * 0.5f, LOW_Y, w1, H);
        x += w1;

        float gap = TILE * 3.0f;

        AddDecorPin(x + 75, -317.0f, true);
        AddDecorPin(x + gap * 0.5f, -317.0f, true);
        AddDecorPin(x + (TILE * 2.5f), -317.0f, true);


        x += gap;

        float w2 = TILE * 5.0f;
        AddPlatform("asset/Field/block.png", x + w2 * 0.5f, LOW_Y, w2, H);

        Tutorial* tutorial1 = new Tutorial();
        tutorial1->Init(
            "asset/Field/Boad.png",
            2500, -230, 200, 200,
            Tutorial::Type::Move,
            true,   //チュートリアル表示ON
            true    //チェックポイントON
        );
        tutorial1->InitTutorialImage(
            "asset/Ui/title.png",
            1000, 750,   //表示サイズ
            300, 450      //看板からのオフセット
        );
        tutorial1->SetRespawnPosition(100, -150, 0);
        m_tutorials.push_back(tutorial1);

        x += w2;
    }

    //3
    {
        float w1 = TILE * 1.0f;
        AddPlatform("asset/Field/block.png", x + w1 * 0.5f, LOW_Y, w1, H);
        x += w1;

        float gap = TILE * 5.0f;

        AddDecorPin(x + TILE * 0.5f, -317.0f, true);
        AddDecorPin(x + TILE * 2.3f, -317.0f, true);
        AddDecorPin(x + TILE * 4.3f, -317.0f, true);

        x += gap;

        float w2 = TILE * 6.0f;
        AddPlatform("asset/Field/block.png", x + w2 * 0.5f + 125, LOW_Y, w2, H);
        x += w2;
    }

    //4
    {
        float w1 = TILE * 2.0f;
        AddPlatform("asset/Field/block.png", x + w1 * 0.5f, LOW_Y, w1, H);
        x += w1;

        float gap = TILE * 2.0f;

        AddPullPin(x + gap +150.0f, -100.0f, false);

        x += gap;

        float w2 = TILE * 10.0f;
        AddPlatform("asset/Field/block.png", x + w2 * 0.5f, LOW_Y, w2, H);
        x += w2;
    }

    //5
    {
        float w1 = TILE * 2.0f;
        AddPlatform("asset/Field/block.png", x + w1 * 0.5f, LOW_Y, w1, H);
        x += w1;


        AddPullPin(x + 120.0f, 40.0f, false);

        float w2 = TILE * 10.0f;
        AddPlatform("asset/Field/block.png", x + w2 * 0.5f, HIGH_Y, w2, H);
        x += w2 + 2*TILE;
    }

    //6
    {
        float w1 = TILE * 1.0f;
        AddPlatform("asset/Field/block.png", x + w1 * 0.5f, HIGH_Y, w1, H);
        x += w1;

        BlockPin* m_targetPin;
        m_targetPin = AddPullPin(x-150.0, -200.0f, true);

        // 足場化（地面として衝突させたい）
        m_collision->AddStatic(m_targetPin->GetObject());  
        m_targetPin->SetForceGround(true);
        m_targetPin->SetLimitPos(x - 150.0, x - 300.0, -200.0, -200.0);
        m_targetPin->SetMoveAxis(BlockPin::MoveAxis::Horizontal);
        m_collision->SetTag(m_targetPin->GetObject(), ColliderTag::Platform);

        float w2 = TILE * 4.0f;
        AddPlatform("asset/Field/block.png", x + w2 * 0.5f, HIGH_Y, w2, H);
        x += w2;
    }

    //7
    {
        float w1 = TILE * 1.0f;
        AddPlatform("asset/Field/block.png", x + w1 * 0.5f, -900, w1, H);
        x += w1;

        {
            
            m_targetPin2->Init("asset/Field/block.png", x + 700.0, -620.0f, 500, 35);
            m_targetPin2->SetCollisionManager(m_collision);
            m_targetPin2->SetcanRollPin(true);
            m_targetPin2->SetcanDecorate(false);
            m_pins.push_back(m_targetPin2);
            m_collision->AddStatic(m_targetPin2->GetObject());
            m_targetPin2->SetForceGround(true);
            m_collision->SetTag(m_targetPin2->GetObject(), ColliderTag::Platform);

            RemoteWindPin* m_hook =new RemoteWindPin;
            m_hook->Init("asset/Field/Pin1.png", x - 100, 150, 35, 35);
            m_hook->SetCollisionManager(m_collision);
            m_hook->SetcanRollPin(false);
            m_hook->SetTarget(m_targetPin2);

            // ガイド
            m_hook->AddGuide({ x + 250, -620, 0 });

            m_pins.push_back(m_hook);
            m_collision->SetTag(m_hook->GetObject(), ColliderTag::Pin);
        }

        x += 500;

        float w2 = TILE * 8.0f;
        AddPlatform("asset/Field/block.png", x + w2 * 0.5f, -900, w2, H);
        x += w2;
    }

    //8
    {
        x += 500;
        
        Pin* JumpPin = new Pin;

        JumpPin->Init("asset/Field/Pin.png", x-250, -300, 35, 35);
        JumpPin->SetCollisionManager(m_collision);
        m_pins.push_back(JumpPin);
        JumpPin->SetcanRollPin(false);
        JumpPin->SetcanDecorate(false);

        float w1 = TILE * 6.0f;
        AddPlatform("asset/Field/block.png", x - 150 + w1 * 0.5f, -900, w1, H);
        x += w1;
    }

    //9
    {
        x += 500;

        Pin* JumpPin = new Pin;

        JumpPin->Init("asset/Field/Pin.png", x - 500, -300, 35, 35);
        JumpPin->SetCollisionManager(m_collision);
        m_pins.push_back(JumpPin);
        JumpPin->SetcanRollPin(false);
        JumpPin->SetcanDecorate(false);

        BlockPin* m_targetPin;
        m_targetPin = AddPullPin(x - 200, -600.0f, true);

        m_collision->AddStatic(m_targetPin->GetObject());
        m_targetPin->SetForceGround(true);
        m_targetPin->SetLimitPos(x - 200, x - 300, -600.0f, -600.0f);
        m_targetPin->SetMoveAxis(BlockPin::MoveAxis::Horizontal);
        m_collision->SetTag(m_targetPin->GetObject(), ColliderTag::Platform);

        float w1 = TILE * 6.0f;
        AddPlatform("asset/Field/block.png", x - 150 + w1 * 0.5f, -900, w1, H);
        x += w1;
    }

    //10
    {
        x += 1000;

        Pin* JumpPin = new Pin;

        JumpPin->Init("asset/Field/Pin.png", x - 900, -300, 35, 35);
        JumpPin->SetCollisionManager(m_collision);
        m_pins.push_back(JumpPin);
        JumpPin->SetcanRollPin(false);
        JumpPin->SetcanDecorate(false);

        {
            BlockPin* m_targetPin2 = new BlockPin;
            m_targetPin2->Init("asset/Field/Pin.png", x + 100.0, -600.0f, 350, 35);
            m_targetPin2->SetCollisionManager(m_collision);
            m_targetPin2->SetcanRollPin(true);
            m_targetPin2->SetcanDecorate(false);
            m_pins.push_back(m_targetPin2);
            m_collision->AddStatic(m_targetPin2->GetObject());
            m_targetPin2->SetForceGround(true);
            m_collision->SetTag(m_targetPin2->GetObject(), ColliderTag::Platform);

            RemoteWindPin* m_hook = new RemoteWindPin;
            m_hook->Init("asset/Field/Pin1.png", x - 1300, -400, 35, 35);
            m_hook->SetCollisionManager(m_collision);
            m_hook->SetcanRollPin(false);
            m_hook->SetTarget(m_targetPin2);

            // ガイド
            m_hook->AddGuide({ x - 500, -600, 0 });

            m_pins.push_back(m_hook);
            m_collision->SetTag(m_hook->GetObject(), ColliderTag::Pin);
        }

        float w1 = TILE * 3.0f;
        AddPlatform("asset/Field/block.png", x - 150 + w1 * 0.5f, -900, w1, H);
        x += w1;
    }

   

    BuildDrawList();

    //HP系初期値
    maxHP = m_player.GetMaxHP();
    currentHP = m_player.GetHP();
}

void TutorialStage::Update()
{
    float dt = 1.0f / 60.0f;

    m_camera.Update(m_player.GetObject()->GetPos());

    DirectX::XMFLOAT3 nowPos;

    nowPos = m_player.GetObject()->GetPos();

    if (nowPos.x < 11500 && nowPos.x>10100) {
        g_cameraPos = { 10600, -300, 0 };

    }

    // プレイヤー更新
    m_player.Update(dt, m_platforms, m_enemies, m_pins);

    const DirectX::XMFLOAT3 p = m_player.GetObject()->GetPos();
    if (p.y < m_fallDeadLineY)
    {
        DirectX::XMFLOAT2 dummyDir = { 0.0f, -1.0f };
        m_player.TakeDamage(999, dummyDir);

        m_isPlayerDead = true;
    }

    // 敵（今回は基本空）
    for (auto& enemy : m_enemies) {
        enemy->Update(dt);

        auto playerAABB = m_collision->GetAABB(m_player.GetObject());
        auto enemyAABB = m_collision->GetAABB(enemy->GetObject());

        if (m_collision->CheckOverlap(playerAABB, enemyAABB))
        {
            DirectX::XMFLOAT3 playerPos = m_player.GetObject()->GetPos();
            DirectX::XMFLOAT3 enemyPos = enemy->GetObject()->GetPos();

            DirectX::XMFLOAT2 knockbackDir = {
                playerPos.x - enemyPos.x,
                playerPos.y - enemyPos.y
            };

            m_player.TakeDamage(1, knockbackDir);
        }
    }

    // ピン更新（BlockPinが動くならAABB更新）
    for (auto* pin : m_pins)
    {
        pin->Update(dt);

        BlockPin* blockPin = dynamic_cast<BlockPin*>(pin);
        if (blockPin)
        {
            m_collision->GetAABB(blockPin->GetObject());
        }
    }

    // 衝突判定
    m_collision->CheckAll();

    auto playerPos = m_player.GetObject()->GetPos();
    for (auto* tutorial : m_tutorials)
    {
        tutorial->Update(dt, playerPos);

        // チェックポイントとして有効化されたら記録
        if (tutorial->IsActivated() && tutorial->IsCheckpoint())
        {
            m_currentCheckpoint = tutorial->GetRespawnPosition();
            m_hasCheckpoint = true;
        }
    }
    // UI追従
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
    default:
        // 0以下は全部暗く（任意）
        m_HP_UI1.SetColor(0.1, 0.1, 0.1, 1.0);
        m_HP_UI2.SetColor(0.1, 0.1, 0.1, 1.0);
        m_HP_UI3.SetColor(0.1, 0.1, 0.1, 1.0);
        break;
    }

    if (m_player.isDead()) {
        m_isPlayerDead = true;
    }
}

void TutorialStage::Draw()
{
    std::sort(
        m_drawList.begin(),
        m_drawList.end(),
        [](const DrawItem& a, const DrawItem& b) {
            return static_cast<int>(a.layer) < static_cast<int>(b.layer);
        }
    );
    
    for (size_t i = 0; i < m_drawList.size(); ++i) {
        Object* obj = m_drawList[i].obj;
        if (!obj) continue;

        obj->Draw(
            g_pDeviceContext,
            g_pInputLayout,
            g_pVertexShader,
            g_pPixelShader,
            g_pConstantBuffer
        );
    }
    // ピンは DebugStage と同様に別途描画
    for (auto* pin : m_pins)
    {
        pin->Draw();
        if (auto* r = dynamic_cast<RemoteWindPin*>(pin))
        {
            r->DrawGuides();
        }
    }

}

void TutorialStage::UnInit()
{
    // Platform
    for (auto& plat : m_platforms) plat->UnInit();
    m_platforms.clear();

    // Enemy
    for (auto& enemy : m_enemies) {
        enemy->UnInit();
        delete enemy;
    }
    m_enemies.clear();

    // Background / Player
    m_background.UnInit();
    m_player.Uninit();

    // Pins（メモリ解放まで行う）
    for (auto* pin : m_pins) {
        pin->UnInit();
        delete pin;
    }
    m_pins.clear();

    for (auto& tutorial : m_tutorials)
    {
        tutorial->UnInit();
        delete tutorial;
    }

    delete m_collision;
    m_collision = nullptr;
}

void TutorialStage::Respawn()
{
    if (!m_hasCheckpoint)
        return;

    //チェックポイント位置にプレイヤーを配置
    m_player.GetObject()->SetPos(
        m_currentCheckpoint.x,
        m_currentCheckpoint.y,
        m_currentCheckpoint.z
    );

    //死亡フラグをリセット
    m_isPlayerDead = false;

}

void TutorialStage::SetResoawnPos(DirectX::XMFLOAT3 respawnpos)
{
    m_currentCheckpoint = respawnpos;
}
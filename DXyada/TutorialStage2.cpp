#include "TutorialStage2.h"
#include "Renderer.h"
#include <algorithm>
#include "Enemy.h"
#include "WingRippa.h"
#include "Hariyama.h"

// 既存と同様に外部にある想定
extern DirectX::XMFLOAT3 g_cameraPos;

void TutorialStage2::AddPlatform(const char* tex, float x, float y, float w, float h)
{
    Platform p= Platform();
    p.Init(tex, x, y, w, h);
    m_platforms.push_back(p);

    Platform back = m_platforms.back();
    m_collision->AddStatic(back.GetObject());
    m_collision->SetTag(back.GetObject(), ColliderTag::Platform);
}

void TutorialStage2::AddDecorPin(float x, float y, bool canDecorate)
{
    Pin* pin = new Pin();
    pin->Init("asset/Field/Pin.png", x, y, 35, 35);
    pin->SetCollisionManager(m_collision);

    pin->SetcanRollPin(true);
    pin->SetcanDecorate(canDecorate);

    m_pins.push_back(pin);
}

BlockPin* TutorialStage2::AddPullPin(float x, float y, bool canRollPin)
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


void TutorialStage2::BuildDrawList()
{
    m_drawList.clear();

    DrawItem item;

    item.obj = &m_background;
    item.layer = DrawLayer::Background;
    m_drawList.push_back(item);

    for (size_t i = 0; i < m_platforms.size(); ++i) {
        item.obj = m_platforms[i].GetObject();
        item.layer = DrawLayer::StageObject;
        m_drawList.push_back(item);
    }

    for (size_t i = 0; i < m_enemies.size(); ++i) {
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

    for (auto* obj : m_player.GetRibbon().GetDrawObjects())
    {
        DrawItem it;
        it.obj = obj;
        it.layer = DrawLayer::BackObject;
        m_drawList.push_back(it);
    }

    item.obj = &m_HP_UI1; item.layer = DrawLayer::UI; m_drawList.push_back(item);
    item.obj = &m_HP_UI2; item.layer = DrawLayer::UI; m_drawList.push_back(item);
    item.obj = &m_HP_UI3; item.layer = DrawLayer::UI; m_drawList.push_back(item);
}

void TutorialStage2::Init()
{
    g_cameraPos = { 0, 0, 0 };

    m_collision = new CollisionManager();

    m_player.Init();
    m_player.SetCollisionManager(m_collision);
    m_player.GetObject()->SetPos(0, 150, 0);

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

    // const float SECTION_GAP = TILE * 0.8f;

    m_platforms.clear();
    m_platforms.reserve(32);

    //1
    {
        float w1 = TILE * 8.0f;
        AddPlatform("asset/Field/block.png", x + w1 * 0.5f, LOW_Y, w1, H);
        x += w1;

        float gap = TILE * 3.0f;

        AddDecorPin(x + gap * 0.5f, -317.0f, true);

        x += gap;

        float w2 = TILE * 5.0f;
        AddPlatform("asset/Field/block.png", x + w2 * 0.5f, LOW_Y, w2, H);

        x += w2;
    }

    //2
    {
        float w1 = TILE * 2.0f;
        AddPlatform("asset/Field/block.png", x + w1 * 0.5f, LOW_Y, w1, H);
        x += w1;

        float gap = TILE * 2.0f;

        AddPullPin(x + gap + 150.0f, -100.0f, false);

        x += gap;

        float w2 = TILE * 5.0f;
        AddPlatform("asset/Field/block.png", x + w2 * 0.5f, LOW_Y, w2, H);
        x += w2;
    }

    //3
    {
        float w1 = TILE * 2.0f;
        AddPlatform("asset/Field/block.png", x + w1 * 0.5f, LOW_Y, w1, H);
        x += w1;


        AddPullPin(x + 120.0f, 40.0f, false);

        float w2 = TILE * 5.0f;
        AddPlatform("asset/Field/block.png", x + w2 * 0.5f, HIGH_Y, w2, H);
        x += w2;
    }

    //4
    {
        float leftW = TILE * 1.0f;
        AddPlatform("asset/Field/block.png", x + leftW * 0.5f, HIGH_Y, leftW, H);
        x += leftW;

        float holeW = 400.0f;
        BlockPin* m_targetPin = AddPullPin(x + holeW * 1.0f, -200.0f, true);

        m_collision->AddStatic(m_targetPin->GetObject());
        m_targetPin->SetForceGround(true);
        m_targetPin->SetLimitPos(x + holeW * 1.0f, x + holeW * 0.5f, -200.0f, -200.0f);
        m_targetPin->SetMoveAxis(BlockPin::MoveAxis::Horizontal);
        m_collision->SetTag(m_targetPin->GetObject(), ColliderTag::Platform);

        x += holeW;

        float rightW = TILE * 4.0f;
        AddPlatform("asset/Field/block.png", x + rightW * 0.5f, HIGH_Y, rightW, H);
        x += rightW;
    }

    //5
    {
        x += 500;

        Pin* JumpPin = new Pin;

        JumpPin->Init("asset/Field/Pin.png", x - 200, 45, 35, 35);
        JumpPin->SetCollisionManager(m_collision);
        m_pins.push_back(JumpPin);
        JumpPin->SetcanRollPin(false);
        JumpPin->SetcanDecorate(false);

        float w1 = TILE * 6.0f;
        AddPlatform("asset/Field/block.png", x - 150 + w1 * 0.5f, HIGH_Y, w1, H);
        x += w1;
    }

    //6
    {
        float sectionW = 600.0f;

        AddPlatform(
            "asset/Field/block.png",
            x + sectionW * 0.5f,
            HIGH_Y,
            sectionW,
            H
        );
        // 羽リッパー反転用・左壁
        Platform wallL;
        wallL.Init(
            "asset/Field/block.png",
            x - 100.0f,
            HIGH_Y + 100.0f,
            -50.0f,
            200.0f
        );
        m_collision->AddStatic(wallL.GetObject());
        m_collision->SetTag(wallL.GetObject(), ColliderTag::Platform);

        // 羽リッパー反転用・右壁
        Platform wallR;
        wallR.Init(
            "asset/Field/block.png",
            x + sectionW + 50.0f,
            HIGH_Y + 100.0f,
            50.0f,
            200.0f
        );
        m_collision->AddStatic(wallR.GetObject());
        m_collision->SetTag(wallR.GetObject(), ColliderTag::Platform);


        {
            WingRippa* rippa = new WingRippa(WingRippa::Type::ZigZag);
            rippa->Init(
                "asset/Field/Wing_Rippa.png",
                x + sectionW * 0.5f,
                HIGH_Y + 550.0f,      // 床の少し上
                150.0f,
                150.0f
            );
            rippa->SetCollisionManager(m_collision);

            m_enemies.push_back(rippa);
            m_collision->AddMoved(rippa->GetObject());
            m_collision->SetTag(rippa->GetObject(), ColliderTag::Enemy);
        }

        x += sectionW;
    }

    //7
    {
        float section7StartX = 7000.0f;
        float section7EndX = 8000.0f;

        float w1 = TILE * 1.0f;
        AddPlatform("asset/Field/block.png", x + w1 * 0.5f, -700, w1, H);
        x += w1;

        {

            m_targetPin2->Init("asset/Field/block.png", x + 580.0, -433.0f, 260, 65);
            m_targetPin2->SetCollisionManager(m_collision);
            m_targetPin2->SetcanRollPin(true);
            m_targetPin2->SetcanDecorate(false);
            m_pins.push_back(m_targetPin2);
            m_collision->AddStatic(m_targetPin2->GetObject());
            m_targetPin2->SetForceGround(true);
            m_collision->SetTag(m_targetPin2->GetObject(), ColliderTag::Platform);

            RemoteWindPin* m_hook = new RemoteWindPin;
            m_hook->Init("asset/Field/Pin1.png", x - 50, 150, 35, 35);
            m_hook->SetCollisionManager(m_collision);
            m_hook->SetcanRollPin(false);
            m_hook->SetTarget(m_targetPin2);

            // ガイド
            m_hook->AddGuide({ x + 130, -433.0, 0 });

            m_pins.push_back(m_hook);
            m_collision->SetTag(m_hook->GetObject(), ColliderTag::Pin);
        }

        x += 500;

        float w2 = TILE * 1.0f;
        AddPlatform("asset/Field/block.png", x + w2 * 0.1f, -700, 350, H);
        x += w2;
    }

    //8
    {
        x += 500;

        Pin* JumpPin = new Pin;

        JumpPin->Init("asset/Field/Pin.png", x - 280, -130, 35, 35);
        JumpPin->SetCollisionManager(m_collision);
        m_pins.push_back(JumpPin);
        JumpPin->SetcanRollPin(false);
        JumpPin->SetcanDecorate(false);

        BlockPin* m_targetPin;
        m_targetPin = AddPullPin(x - 70, -418.0f, true);

        m_collision->AddStatic(m_targetPin->GetObject());
        m_targetPin->SetForceGround(true);
        m_targetPin->SetLimitPos(x, x - 110, -0.0f, -600.0f);
        m_targetPin->SetMoveAxis(BlockPin::MoveAxis::Horizontal);
        m_collision->SetTag(m_targetPin->GetObject(), ColliderTag::Platform);

        float w1 = TILE * 6.0f;
        AddPlatform("asset/Field/block.png", x + w1 * 0.45f, -700, w1, H);
        x += w1;
    }

    //9
    {
        x += 1000;

        Pin* JumpPin = new Pin;

        JumpPin->Init("asset/Field/Pin.png", x - 750, -100, 35, 35);
        JumpPin->SetCollisionManager(m_collision);
        m_pins.push_back(JumpPin);
        JumpPin->SetcanRollPin(false);
        JumpPin->SetcanDecorate(false);

        {
            BlockPin* m_targetPin2 = new BlockPin;
            m_targetPin2->Init("asset/Field/block.png", x + 00.0, -433.0f, 650, 65);
            m_targetPin2->SetCollisionManager(m_collision);
            m_targetPin2->SetcanRollPin(true);
            m_targetPin2->SetcanDecorate(false);
            m_pins.push_back(m_targetPin2);
            m_collision->AddStatic(m_targetPin2->GetObject());
            m_targetPin2->SetForceGround(true);
            m_collision->SetTag(m_targetPin2->GetObject(), ColliderTag::Platform);

            RemoteWindPin* m_hook = new RemoteWindPin;
            m_hook->Init("asset/Field/Pin1.png", x - 1300, -230, 35, 35);
            m_hook->SetCollisionManager(m_collision);
            m_hook->SetcanRollPin(false);
            m_hook->SetTarget(m_targetPin2);

            // ガイド
            m_hook->AddGuide({ x - 300, -433.0, 0 });

            m_pins.push_back(m_hook);
            m_collision->SetTag(m_hook->GetObject(), ColliderTag::Pin);
        }

        float w1 = TILE * 3.0f;
        AddPlatform("asset/Field/block.png", x - 150 + w1 * 0.5f, -700, w1, H);
        x += w1;
    }
    //10
    {

    }


    BuildDrawList();

    // HP系初期値
    maxHP = m_player.GetMaxHP();
    currentHP = m_player.GetHP();
}

void TutorialStage2::Update()
{
    float dt = 1.0f / 60.0f;

    DirectX::XMFLOAT3 playerPos = m_player.GetObject()->GetPos();

    // ===== トリガー =====
    if (playerPos.x > 7000.0f)// && playerPos.x < 8500.0f
    {
        m_cameraTargetOffset = -150.0f;
    }
    else
    {
        m_cameraTargetOffset = 0.0f;
    }

    // ===== オフセット補間 =====
    const float lerpSpeed = 0.05f;
    m_cameraYOffset += (m_cameraTargetOffset - m_cameraYOffset) * lerpSpeed;

    // ===== カメラに渡す位置を作る =====
    DirectX::XMFLOAT3 cameraFollowPos = playerPos;
    cameraFollowPos.y += m_cameraYOffset;

    // ===== カメラ更新 =====
    m_camera.Update(cameraFollowPos);



    DirectX::XMFLOAT3 nowPos;

    nowPos = m_player.GetObject()->GetPos();

    /*  if (nowPos.x < 11500 && nowPos.x>10100) {
          g_cameraPos = { 10600, -300, 0 };

      }*/

      // プレイヤー更新
    m_player.Update(dt, m_platforms, m_enemies, m_pins);

    const DirectX::XMFLOAT3 p = m_player.GetObject()->GetPos();
    if (p.y < m_fallDeadLineY)
    {
        DirectX::XMFLOAT2 dummyDir = { 0.0f, -1.0f };
        m_player.TakeDamage(999, dummyDir);

        m_isPlayerDead = true;
    }

    // 敵
    for (auto& enemy : m_enemies) {
        enemy->Update(dt);

        auto playerAABB = m_collision->GetAABB(m_player.GetObject());
        auto enemyAABB = m_collision->GetAABB(enemy->GetObject());

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

void TutorialStage2::Draw()
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

void TutorialStage2::UnInit()
{
    // Platform
    for (auto& plat : m_platforms) plat.UnInit();
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

    delete m_collision;
    m_collision = nullptr;
}

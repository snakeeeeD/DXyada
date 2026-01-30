#include "Stage2.h"    
#include "Renderer.h"
#include <algorithm>
#include "sound.h"

extern Sound* g_sound;
extern DirectX::XMFLOAT3 g_cameraPos;

void TutorialStage2::RegisterEnemyMarker(Enemy* enemy, const char* markerTex, bool isMove)
{
    DecoLinkEnemy el;
    el.enemy = enemy;
    el.marker.Init(markerTex);
    el.marker.SetMoveMode(isMove);
    el.marker.Hide();
    m_enemyMarkers.push_back(el);
}

void TutorialStage2::RegisterPinMarker(Pin* pin, const char* markerTex, bool isMove)
{
    if (!pin) return;

    DecoLinkPin link;
    link.pin = pin;
    link.marker.Init(markerTex);
    link.marker.SetMoveMode(isMove);
    link.marker.Hide();
    m_pinMarkers.push_back(link);
}

void TutorialStage2::AddPlatform(const char* tex, float x, float y, float w, float h)
{
    Platform p;
    p.Init(tex, x, y, w, h);
    m_platforms.push_back(p);

    Platform& back = m_platforms.back();
    m_collision->AddStatic(back.GetObject());
    m_collision->SetTag(back.GetObject(), ColliderTag::Platform);
}

void TutorialStage2::AddDecorPin(float x, float y, bool canDecorate)
{
    Pin* pin = new Pin();
    pin->Init("asset/Field/PinDeco.png", x, y, 35, 35); 
    pin->SetCollisionManager(m_collision);

    pin->SetcanRollPin(true);
    pin->SetcanDecorate(canDecorate);
    pin->SetPinKind(PinKind::Deco);

    m_pins.push_back(pin);
    m_collision->SetTag(pin->GetObject(), ColliderTag::Pin);

    RegisterPinMarker(pin, "asset/Field/星 ワッペン.png", false);
}

BlockPin* TutorialStage2::AddPullPin(float x, float y, bool canRollPin)
{
    BlockPin* pin = new BlockPin();
    pin->SetCollisionManager(m_collision);

    pin->SetcanRollPin(canRollPin);
    pin->SetcanDecorate(false);

    if (canRollPin)
    {
        pin->Init("asset/Field/PinRoll.png", x, y, 35, 35);
        pin->SetPinKind(PinKind::Roll);
    }
    else
    {
        pin->Init("asset/Field/PinPulled.png", x, y, 35, 35);
        pin->SetPinKind(PinKind::Pulled);
    }

    m_pins.push_back(pin);
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

    for (size_t i = 0; i < m_platforms.size(); ++i)
    {
        item.obj = m_platforms[i].GetObject();
        item.layer = DrawLayer::StageObject;
        m_drawList.push_back(item);
    }

    item.obj = m_player.GetDecoratingEffectBack();
    item.layer = DrawLayer::Enemy;
    m_drawList.push_back(item);

    for (auto& em : m_enemyMarkers)
    {
        item.obj = em.marker.GetObject();
        item.layer = DrawLayer::Enemy;
        m_drawList.push_back(item);

        if (em.marker.GetisMove() == false)
        {
            item.obj = em.marker.GetObject2();
            item.layer = DrawLayer::Enemy;
            m_drawList.push_back(item);

            item.obj = em.marker.GetObject3();
            item.layer = DrawLayer::Enemy;
            m_drawList.push_back(item);
        }
    }

    for (auto& pm : m_pinMarkers)
    {
        item.obj = pm.marker.GetObject();
        item.layer = DrawLayer::Enemy;
        m_drawList.push_back(item);

    }


    for (size_t i = 0; i < m_enemies.size(); ++i)
    {
        item.obj = m_enemies[i]->GetObject();
        item.layer = DrawLayer::Enemy;
        m_drawList.push_back(item);
    }

    for (size_t i = 0; i < m_pins.size(); ++i)
    {
        item.obj = m_pins[i]->GetMarkObject();
        item.layer = DrawLayer::Enemy;
        m_drawList.push_back(item);

        item.obj = m_pins[i]->GetMarkObject2();
        item.layer = DrawLayer::Enemy;
        m_drawList.push_back(item);

        item.obj = m_pins[i]->GetMarkObject3();
        item.layer = DrawLayer::Enemy;
        m_drawList.push_back(item);

        item.obj = m_pins[i]->GetObject();
        item.layer = DrawLayer::StageObject;
        m_drawList.push_back(item);
    }

    for (size_t i = 0; i < m_tutorials.size(); ++i)
    {
        item.obj = m_tutorials[i]->GetObject();
        item.layer = DrawLayer::BackObject;
        m_drawList.push_back(item);

        item.obj = m_tutorials[i]->GetDisplayObject();
        item.layer = DrawLayer::BackObject;
        m_drawList.push_back(item);
    }

    item.obj = m_player.GetObject();
    item.layer = DrawLayer::Player;
    m_drawList.push_back(item);

    item.obj = m_player.GetDecoratingEffectFront();
    item.layer = DrawLayer::Player;
    m_drawList.push_back(item);

    item.obj = m_player.GetGuideLineObject();
    item.layer = DrawLayer::Player;
    m_drawList.push_back(item);

    item.obj = m_player.GetCircle();
    item.layer = DrawLayer::Player;
    m_drawList.push_back(item);

    item.obj = m_player.GetJustkiran();
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

void TutorialStage2::Init()
{
    g_cameraPos = { 0, 0, 0 };

    m_collision = new CollisionManager();

    m_player.Init();
    m_player.SetCollisionManager(m_collision);
    m_player.GetObject()->SetPos(0, 150, 0);

    m_isGoalReached = false;
    m_isPlayerDead = false;

    m_currentCheckpoint = { 0, 150, 0 };
    m_hasCheckpoint = true;

    m_background.Init();
    m_background.AddTexture("asset/Field/ステージ背景.png");
    m_background.SetPos(7200.0f, 0.0f, 0.0f);
    m_background.SetSize(23000.0f, 2100.0f, 0.0f);
    m_background.SetUVMode(Object::UVMode::Loop);
    m_background.SetUVLength(5.0f);
    m_background.SetUVScroll(0.0f);

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

    const char* FLOOR_TEX = "asset/Field/床ブロック.png";

    m_platforms.clear();
    m_enemies.clear();
    m_pins.clear();
    m_tutorials.clear();
    m_goal.clear();

    m_pinMarkers.clear();
    m_enemyMarkers.clear();
    m_drawList.clear();

    m_platforms.reserve(32);

    {
        float w1 = TILE * 8.0f;
        AddPlatform(FLOOR_TEX, x + w1 * 0.5f, LOW_Y, w1, H);
        x += w1;

        Tutorial* tutorial1 = new Tutorial();
        tutorial1->Init(
            "asset/Field/Boad.png",
            1, -230, 0, 0,
            Tutorial::Type::Move,
            false,
            true
        );
        tutorial1->SetRespawnPosition(1, -150, 0);
        m_tutorials.push_back(tutorial1);

        float gap = TILE * 3.0f;
        AddDecorPin(x + gap * 0.5f, -317.0f, true);

        x += gap;

        float w2 = TILE * 5.0f;
        AddPlatform(FLOOR_TEX, x + w2 * 0.5f, LOW_Y, w2, H);
        x += w2;
    }

    {
        float w1 = TILE * 2.0f;
        AddPlatform(FLOOR_TEX, x + w1 * 0.5f, LOW_Y, w1, H);
        x += w1;

        Tutorial* tutorial = new Tutorial();
        tutorial->Init(
            "asset/Field/Boad.png",
            x - 200, -230, 200, 200,
            Tutorial::Type::Move,
            true,
            true
        );
        tutorial->InitTutorialImage("asset/Field/チュートリアル看板2-2.png", 1000, 500, 300, 450);
        tutorial->SetRespawnPosition(x - 200, -150, 0);
        m_tutorials.push_back(tutorial);

        float gap = TILE * 2.0f;
        AddPullPin(x + gap + 150.0f, -100.0f, false);

        x += gap;

        float w2 = TILE * 5.0f;
        AddPlatform(FLOOR_TEX, x + w2 * 0.5f, LOW_Y, w2, H);
        x += w2;
    }

    {
        float w1 = TILE * 2.0f;
        AddPlatform(FLOOR_TEX, x + w1 * 0.5f, LOW_Y, w1, H);
        x += w1;

        AddPullPin(x + 120.0f, 40.0f, false);

        float w2 = TILE * 5.0f;
        AddPlatform(FLOOR_TEX, x + w2 * 0.5f, HIGH_Y, w2, H);
        x += w2;
    }

    {
        float leftW = TILE * 1.0f;
        AddPlatform(FLOOR_TEX, x + leftW * 0.5f, HIGH_Y, leftW, H);
        x += leftW;

        Tutorial* tutorial = new Tutorial();
        tutorial->Init(
            "asset/Field/Boad.png",
            x - 200, -80, 200, 200,
            Tutorial::Type::Move,
            true,
            true
        );
        tutorial->InitTutorialImage("asset/Field/チュートリアル看板2-4.png", 1000, 500, 300, 450);
        tutorial->SetRespawnPosition(x - 200, -150, 0);
        m_tutorials.push_back(tutorial);

        float holeW = 400.0f;

        BlockPin* targetPin = AddPullPin(x + holeW * 1.0f - 20, -200.0f, true);

        targetPin->SetForceGround(true);
        targetPin->SetLimitPos(x + holeW * 1.0f, x + holeW * 0.5f, -200.0f, -200.0f);
        targetPin->SetMoveAxis(BlockPin::MoveAxis::Horizontal);

        m_collision->SetTag(targetPin->GetObject(), ColliderTag::Platform);

        m_pullLimitWatchPin = targetPin;
        m_pullLimitFired = false;
        m_pullLimitAxis = BlockPin::MoveAxis::Horizontal;

        m_pullLimitMinX = x + holeW * 0.5f;
        m_pullLimitMaxX = x + holeW * 1.0f;
        m_pullLimitMinY = -200.0f;
        m_pullLimitMaxY = -200.0f;

        RegisterPinMarker(targetPin, "asset/Field/リボン ワッペン.png", true);

        x += holeW;

        float rightW = TILE * 2.0f;
        AddPlatform(FLOOR_TEX, x + rightW * 0.5f, HIGH_Y, rightW, H);
        x += rightW;
    }

    {
        float sectionW = 1200.0f;
        AddPlatform(FLOOR_TEX, x + sectionW * 0.5f, HIGH_Y, sectionW, H);

        Rippa* rippa = new Rippa(Rippa::Type::Wandering);
        rippa->Init("asset/Field/rippa.png", x + 700, HIGH_Y + 360, 140, 150);
        rippa->SetCollisionManager(m_collision);
        m_enemies.push_back(rippa);
        RegisterEnemyMarker(rippa, "asset/Field/フェルトワッペン リッパー.png", true);

        WingRippa* wirippa = new WingRippa(WingRippa::Type::ZigZag);
        wirippa->Init("asset/Field/Wing_Rippa.png", x + 900, HIGH_Y + 550, 150, 150);
        wirippa->SetCollisionManager(m_collision);
        m_enemies.push_back(wirippa);
        RegisterEnemyMarker(wirippa, "asset/Field/フェルトワッペン 羽リッパー.png", true);

        x += sectionW;
    }

    {
        float w1 = TILE * 1.0f;
        AddPlatform(FLOOR_TEX, x + w1 * 0.5f, -700, w1, H);
        x += w1;

        {
            BlockPin* target = new BlockPin;
            target->Init("asset/Field/フェルトブロック.png", x + 580.0f, -433.0f, 360, 65);
            target->SetCollisionManager(m_collision);
            target->SetcanRollPin(true);
            target->SetcanDecorate(false);
            m_pins.push_back(target);

            m_collision->AddStatic(target->GetObject());
            target->SetForceGround(true);
            m_collision->SetTag(target->GetObject(), ColliderTag::Platform);

            RemoteWindPin* hook = new RemoteWindPin;
            hook->Init("asset/Field/PinRemote.png", x - 50, 150, 35, 35);
            hook->SetCollisionManager(m_collision);
            hook->SetcanRollPin(false);
            hook->SetTarget(target);
            hook->SetPinKind(PinKind::RemoteDeco);
            hook->AddGuide({ x + 170, -433.0f, 0 });

            m_pins.push_back(hook);
            m_collision->SetTag(hook->GetObject(), ColliderTag::Pin);

            RegisterPinMarker(hook, "asset/Field/星 ワッペン.png", true);
        }

        x += 500;

        AddPlatform(FLOOR_TEX, x + 35.0f, -700, 350, H);
        x += TILE * 1.0f;
    }

    {
        x += 500;

        float w1 = TILE * 6.0f;

        Pin* JumpPin = new Pin;
        JumpPin->Init("asset/Field/PinJump.png", x - 280, -130, 35, 35);
        JumpPin->SetCollisionManager(m_collision);
        JumpPin->SetcanRollPin(false);
        JumpPin->SetcanDecorate(false);
        JumpPin->SetPinKind(PinKind::Jump);

        m_pins.push_back(JumpPin);
        m_collision->SetTag(JumpPin->GetObject(), ColliderTag::Pin);

        RegisterPinMarker(JumpPin, "asset/Field/リボンちゃん ワッペン.png", true);

        BlockPin* target = AddPullPin(x - 70, -418.0f, true);
        target->SetForceGround(true);
        target->SetLimitPos(x, x - 150, -418.0f, -418.0f);
        target->SetMoveAxis(BlockPin::MoveAxis::Horizontal);
        m_collision->SetTag(target->GetObject(), ColliderTag::Platform);

        m_pullLimitWatchPin = target;
        m_pullLimitFired = false;
        m_pullLimitAxis = BlockPin::MoveAxis::Horizontal;

        m_pullLimitMinX = x ;
        m_pullLimitMaxX = x -150;
        m_pullLimitMinY = -200.0f;
        m_pullLimitMaxY = -200.0f;

        RegisterPinMarker(target, "asset/Field/リボン ワッペン.png", true);

        AddPlatform(FLOOR_TEX, x + w1 * 0.45f, -700, w1, H);
        x += w1;
    }

    {
        const float platformCenterX = x;
        const float platformTopY = LOW_Y + (H * 0.5f);

        GoalPin* goalPin = new GoalPin();
        goalPin->Init(
            "asset/Field/Goal.png",
            platformCenterX,
            platformTopY + (35.0f * 0.5f),
            100.0f,
            300.0f
        );
        goalPin->SetCollisionManager(m_collision);

        m_pins.push_back(goalPin);
        m_collision->SetTag(goalPin->GetObject(), ColliderTag::Pin);
        m_goal.push_back(goalPin->GetGoal());
    }

    BuildDrawList();

    maxHP = m_player.GetMaxHP();
    currentHP = m_player.GetHP();
}

void TutorialStage2::Update()
{
    const float dt = 1.0f / 60.0f;

   
    {
        const DirectX::XMFLOAT3 playerPos = m_player.GetObject()->GetPos();

        if (playerPos.x > 6000.0f)
            m_cameraTargetOffset = -150.0f;
        else
            m_cameraTargetOffset = 0.0f;

        const float lerpSpeed = 0.05f;
        m_cameraYOffset += (m_cameraTargetOffset - m_cameraYOffset) * lerpSpeed;

        DirectX::XMFLOAT3 cameraFollowPos = playerPos;
        cameraFollowPos.y += m_cameraYOffset;

        m_camera.Update(cameraFollowPos);
        m_cameraNowPos = playerPos;

        if (m_cameraNowPos.y < -500.0f)
            g_cameraPos = { g_cameraPos.x, -200.0f, g_cameraPos.z };
    }
    m_player.Update(dt, m_platforms, m_enemies, m_pins);

    {
        const DirectX::XMFLOAT3 p = m_player.GetObject()->GetPos();
        if (p.y < m_fallDeadLineY)
        {
            const int hpBefore = m_player.GetHP();
            DirectX::XMFLOAT2 dummyDir = { 0.0f, -1.0f };

            m_player.TakeDamage(1, dummyDir);
            if (hpBefore > 1) g_sound->Play(SOUND_LABEL_SE_Fall);

            Respawn();
        }
    }

    for (size_t i = 0; i < m_pins.size(); ++i)
    {
        Pin* pin = m_pins[i];
        if (!pin) continue;

        pin->Update(dt);

        if (BlockPin* blockPin = dynamic_cast<BlockPin*>(pin))
            m_collision->GetAABB(blockPin->GetObject());
    }

   
    m_collision->CheckAll();

    {
        const DirectX::XMFLOAT3 playerPos = m_player.GetObject()->GetPos();

        for (size_t i = 0; i < m_tutorials.size(); ++i)
        {
            Tutorial* tutorial = m_tutorials[i];
            if (!tutorial) continue;

            tutorial->Update(dt, playerPos);

            if (tutorial->IsActivated() && tutorial->IsCheckpoint())
            {
                m_currentCheckpoint = tutorial->GetRespawnPosition();
                m_hasCheckpoint = true;
            }
        }
    }

    {
        const DirectX::XMFLOAT3 playerPos = m_player.GetObject()->GetPos();

        for (size_t i = 0; i < m_goal.size(); ++i)
        {
            Goal* goal = m_goal[i];
            if (!goal) continue;

            goal->Update(dt, playerPos);
            if (goal->IsReached())
                m_isGoalReached = true;
        }
    }

    
    
        for (auto& enemy : m_enemies) {
            if (!enemy) continue;

            enemy->Update(dt);

            if (Rippa* rippa = dynamic_cast<Rippa*>(enemy))
            {
                rippa->CheckEnemyCollision(m_enemies);
            }

            NeedleFloor* needleflr = dynamic_cast<NeedleFloor*>(enemy);

            if (enemy->IsDead())
            {
                continue;
            }


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

                if (Rippa* rippa = dynamic_cast<Rippa*>(enemy))
                {
                    m_player.TakeDamage(1, knockbackDir);
                    g_sound->Play(SOUND_LABEL_SE_Damage);

                }
                else if (needleflr->GetState() != NeedleFloor::State::Decorated)
                {
                    m_player.TakeDamage(1, knockbackDir);
                    g_sound->Play(SOUND_LABEL_SE_Damage);

                }


            }
        }


    currentHP = m_player.GetHP();

    m_HP_UI1.SetPos(g_cameraPos.x - 800, g_cameraPos.y + 400, 0);
    m_HP_UI2.SetPos(g_cameraPos.x - 600, g_cameraPos.y + 400, 0);
    m_HP_UI3.SetPos(g_cameraPos.x - 400, g_cameraPos.y + 400, 0);

    switch (currentHP)
    {
    case 3:
        m_HP_UI1.SetColor(1, 1, 1, 1);
        m_HP_UI2.SetColor(1, 1, 1, 1);
        m_HP_UI3.SetColor(1, 1, 1, 1);
        break;
    case 2:
        m_HP_UI1.SetColor(1, 1, 1, 1);
        m_HP_UI2.SetColor(1, 1, 1, 1);
        m_HP_UI3.SetColor(0.1f, 0.1f, 0.1f, 1);
        break;
    case 1:
        m_HP_UI1.SetColor(1, 1, 1, 1);
        m_HP_UI2.SetColor(0.1f, 0.1f, 0.1f, 1);
        m_HP_UI3.SetColor(0.1f, 0.1f, 0.1f, 1);
        break;
    default:
        m_HP_UI1.SetColor(0.1f, 0.1f, 0.1f, 1);
        m_HP_UI2.SetColor(0.1f, 0.1f, 0.1f, 1);
        m_HP_UI3.SetColor(0.1f, 0.1f, 0.1f, 1);
        break;
    }
    if (m_isGoalReached == true)
    {
        m_HP_UI1.SetColor(0.0f, 0.0f, 0.0f, 0);
        m_HP_UI2.SetColor(0.0f, 0.0f, 0.0f, 0);
        m_HP_UI3.SetColor(0.0f, 0.0f, 0.0f, 0);
    }
   
    for (size_t i = 0; i < m_pinMarkers.size(); ++i)
    {
        pm.marker.Update(dt, pm.pin ? pm.pin->GetObject() : nullptr);

        if (!pm.pin) continue;

        const Pin::MarkerEvent ev = pm.pin->ConsumeMarkerEvent();
        if (ev == Pin::MarkerEvent::None) continue;

        pm.marker.ShowAtHead(pm.pin->GetObject(), 120.0f, 300.0f, 200.0f);
    }

    for (auto& em : m_enemyMarkers)
    {
        em.marker.Update(dt, em.enemy ? em.enemy->GetObject() : nullptr);

        if (em.enemy && em.enemy->ConsumeJustDecorated())
        {
            em.marker.ShowAtHead(em.enemy->GetObject(), 180.0f, 300.0f, 200.0f);
        }
    }

    if (m_player.isDead())
        m_isPlayerDead = true;

    if (m_pullLimitWatchPin && !m_pullLimitFired)
    {
        Object* obj = m_pullLimitWatchPin->GetObject();
        if (obj)
        {
            const DirectX::XMFLOAT3 p = obj->GetPos();

            const float eps = 0.5f; 
            bool reached = false;

            if (m_pullLimitAxis == BlockPin::MoveAxis::Horizontal)
            {
                if (p.x <= m_pullLimitMinX + eps || p.x >= m_pullLimitMaxX - eps)
                    reached = true;
            }
            else
            {
                if (p.y <= m_pullLimitMinY + eps || p.y >= m_pullLimitMaxY - eps)
                    reached = true;
            }

            if (reached)
            {
                m_pullLimitFired = true;

                for (auto& pm : m_pinMarkers)
                {
                    if (pm.pin == m_pullLimitWatchPin)
                    {
                        pm.marker.ShowAtHead(pm.pin->GetObject(), 120.0f, 300.0f, 200.0f);
                        break;
                    }
                }
            }
        }
    }
}

void TutorialStage2::Draw()
{
    std::sort(
        m_drawList.begin(),
        m_drawList.end(),
        [](const DrawItem& a, const DrawItem& b)
        {
            return static_cast<int>(a.layer) < static_cast<int>(b.layer);
        }
    );

    for (size_t i = 0; i < m_drawList.size(); ++i)
    {
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
}

void TutorialStage2::UnInit()
{
    for (size_t i = 0; i < m_platforms.size(); ++i)
        m_platforms[i].UnInit();
    m_platforms.clear();

    for (size_t i = 0; i < m_enemies.size(); ++i)
    {
        if (!m_enemies[i]) continue;
        m_enemies[i]->UnInit();
        delete m_enemies[i];
    }
    m_enemies.clear();

    m_background.UnInit();
    m_player.Uninit();

    for (size_t i = 0; i < m_pins.size(); ++i)
    {
        if (!m_pins[i]) continue;
        m_pins[i]->UnInit();
        delete m_pins[i];
    }
    m_pins.clear();

    for (size_t i = 0; i < m_tutorials.size(); ++i)
    {
        if (!m_tutorials[i]) continue;
        m_tutorials[i]->UnInit();
        delete m_tutorials[i];
    }
    m_tutorials.clear();

    m_goal.clear();

    // Markers
    m_pinMarkers.clear();
    m_enemyMarkers.clear();
    m_drawList.clear();

    delete m_collision;
    m_collision = nullptr;
}

void TutorialStage2::Respawn()
{
    if (!m_hasCheckpoint) return;

    m_player.SetInvincible(false);

    m_player.GetObject()->SetPos(
        m_currentCheckpoint.x,
        m_currentCheckpoint.y,
        m_currentCheckpoint.z
    );

    m_isPlayerDead = false;
}

void TutorialStage2::SetResoawnPos(DirectX::XMFLOAT3 respawnpos)
{
    m_currentCheckpoint = respawnpos;
}

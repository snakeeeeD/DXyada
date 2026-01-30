#include "Stage2.h"      // ←あなたのプロジェクト側で TutorialStage2 を宣言してるヘッダ名に合わせて
#include "Renderer.h"
#include <algorithm>
#include "sound.h"

extern Sound* g_sound;
extern DirectX::XMFLOAT3 g_cameraPos;

//==================================================
// Marker registration (Stage1互換)
//==================================================
void TutorialStage2::RegisterEnemyMarker(Enemy* enemy, const char* markerTex, bool isMove)
{
    DecoLinkEnemy el;
    el.enemy = enemy;
    el.marker.Init(markerTex);
    el.marker.SetMoveMode(isMove);
    el.marker.Hide();
    m_enemyMarkers.push_back(el);
}

//==================================================
// Stage building helpers
//==================================================
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
    pin->Init("asset/Field/PinDeco.png", x, y, 35, 35); // Stage1と合わせる
    pin->SetCollisionManager(m_collision);

    pin->SetcanRollPin(true);
    pin->SetcanDecorate(canDecorate);
    pin->SetPinKind(PinKind::Deco);

    m_pins.push_back(pin);

    // --- Pin marker（Stage1と同じ）
    DecoLinkPin link;
    link.pin = pin;
    link.marker.Init("asset/Field/bbbbb.png"); // 好きなのに差し替えてOK
    link.marker.Hide();
    m_pinMarkers.push_back(link);

    m_collision->SetTag(pin->GetObject(), ColliderTag::Pin);
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

//==================================================
// Draw list
//==================================================
void TutorialStage2::BuildDrawList()
{
    m_drawList.clear();

    DrawItem item;

    // Background
    item.obj = &m_background;
    item.layer = DrawLayer::Background;
    m_drawList.push_back(item);

    // Platforms
    for (size_t i = 0; i < m_platforms.size(); ++i)
    {
        item.obj = m_platforms[i].GetObject();
        item.layer = DrawLayer::StageObject;
        m_drawList.push_back(item);
    }

    // Player effects (back)
    item.obj = m_player.GetDecoratingEffectBack();
    item.layer = DrawLayer::Enemy;
    m_drawList.push_back(item);

    // --- Enemy marker（Stage1と同じ）---
    for (size_t i = 0; i < m_enemyMarkers.size(); ++i)
    {
        item.obj = m_enemyMarkers[i].marker.GetObject();
        item.layer = DrawLayer::BackObject;
        m_drawList.push_back(item);

    }

    // --- Pin marker（Stage1と同じ）---
    for (size_t i = 0; i < m_pinMarkers.size(); ++i)
    {
        item.obj = m_pinMarkers[i].marker.GetObject();
        item.layer = DrawLayer::BackObject;
        m_drawList.push_back(item);
    }

    // Enemies
    for (size_t i = 0; i < m_enemies.size(); ++i)
    {
        item.obj = m_enemies[i]->GetObject();
        item.layer = DrawLayer::Enemy;
        m_drawList.push_back(item);
    }

    // Pins（Pin自身のmarkObjectも残すなら）
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

    // Tutorials（あなたの好みで BackObject/StageObject を揃える）
    for (size_t i = 0; i < m_tutorials.size(); ++i)
    {
        item.obj = m_tutorials[i]->GetObject();
        item.layer = DrawLayer::BackObject;
        m_drawList.push_back(item);

        item.obj = m_tutorials[i]->GetDisplayObject();
        item.layer = DrawLayer::BackObject;
        m_drawList.push_back(item);
    }

    // Goals
    for (size_t i = 0; i < m_goal.size(); ++i)
    {
        item.obj = m_goal[i]->GetObject();
        item.layer = DrawLayer::BackObject;
        m_drawList.push_back(item);
    }

    // Player
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

    // Ribbon draw objects
    {
        const std::vector<Object*>& ribbonObjs = m_player.GetRibbon().GetDrawObjects();
        for (size_t i = 0; i < ribbonObjs.size(); ++i)
        {
            DrawItem it;
            it.obj = ribbonObjs[i];
            it.layer = DrawLayer::Player;
            m_drawList.push_back(it);
        }
    }

    // UI
    item.obj = &m_HP_UI1; item.layer = DrawLayer::UI; m_drawList.push_back(item);
    item.obj = &m_HP_UI2; item.layer = DrawLayer::UI; m_drawList.push_back(item);
    item.obj = &m_HP_UI3; item.layer = DrawLayer::UI; m_drawList.push_back(item);
}

//==================================================
// Init
//==================================================
void TutorialStage2::Init()
{
    g_cameraPos = { 0, 0, 0 };

    m_collision = new CollisionManager();

    // Player
    m_player.Init();
    m_player.SetCollisionManager(m_collision);
    m_player.GetObject()->SetPos(0, 250, 0);

    m_isGoalReached = false;
    m_isPlayerDead = false;

    // Checkpoint
    m_currentCheckpoint = { 0, 150, 0 };
    m_hasCheckpoint = true;

    // Background (Stage1と同じ)
    m_background.Init();
    m_background.AddTexture("asset/Field/ステージ背景.png");
    m_background.SetPos(7200.0f, 0.0f, 0.0f);
    m_background.SetSize(23000.0f, 2100.0f, 0.0f);
    m_background.SetUVMode(Object::UVMode::Loop);
    m_background.SetUVLength(5.0f);
    m_background.SetUVScroll(0.0f);

    // UI
    m_HP_UI1.Init(); m_HP_UI1.AddTexture("asset/UI/cursor.png"); m_HP_UI1.SetSize(200, 200, 0);
    m_HP_UI2.Init(); m_HP_UI2.AddTexture("asset/UI/cursor.png"); m_HP_UI2.SetSize(200, 200, 0);
    m_HP_UI3.Init(); m_HP_UI3.AddTexture("asset/UI/cursor.png"); m_HP_UI3.SetSize(200, 200, 0);

    // Collision: player
    m_collision->AddDynamic(m_player.GetObject());
    m_collision->SetTag(m_player.GetObject(), ColliderTag::Player);

    // Stage build constants
    const float TILE = 150.0f;
    const float H = 600.0f;

    const float LOW_Y = -600.0f;
    const float HIGH_Y = LOW_Y + TILE;

    float x = -300.0f;

    // Stage1と同じ床テクスチャに統一
    const char* FLOOR_TEX = "asset/Field/床ブロック.png";

    // clear
    m_platforms.clear();
    m_enemies.clear();
    m_pins.clear();
    m_tutorials.clear();
    m_goal.clear();
  //  m_spawnQueue.clear();
    m_pinMarkers.clear();
    m_enemyMarkers.clear();
    m_drawList.clear();
    m_platforms.reserve(32);

    //========================
    // 1
    //========================
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

    //========================
    // 2
    //========================
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
        tutorial->InitTutorialImage("asset/Ui/title.png", 1000, 750, 300, 450);
        tutorial->SetRespawnPosition(x - 200, -150, 0);
        m_tutorials.push_back(tutorial);

        float gap = TILE * 2.0f;
        AddPullPin(x + gap + 150.0f, -100.0f, false);

        x += gap;

        float w2 = TILE * 5.0f;
        AddPlatform(FLOOR_TEX, x + w2 * 0.5f, LOW_Y, w2, H);
        x += w2;
    }

    //========================
    // 3
    //========================
    {
        float w1 = TILE * 2.0f;
        AddPlatform(FLOOR_TEX, x + w1 * 0.5f, LOW_Y, w1, H);
        x += w1;

        AddPullPin(x + 120.0f, 40.0f, false);

        float w2 = TILE * 5.0f;
        AddPlatform(FLOOR_TEX, x + w2 * 0.5f, HIGH_Y, w2, H);
        x += w2;
    }

    //========================
    // 4 (動くBlockPinを足場扱いにする例)
    //========================
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
        tutorial->InitTutorialImage("asset/Ui/title.png", 1000, 750, 300, 450);
        tutorial->SetRespawnPosition(x - 200, -150, 0);
        m_tutorials.push_back(tutorial);

        float holeW = 400.0f;
        BlockPin* targetPin = AddPullPin(x + holeW * 1.0f - 20, -200.0f, true);

        targetPin->SetForceGround(true);
        targetPin->SetLimitPos(x + holeW * 1.0f, x + holeW * 0.5f, -200.0f, -200.0f);
        targetPin->SetMoveAxis(BlockPin::MoveAxis::Horizontal);

        // 足場扱いにするならタグ変更（必要なら AddStatic も）
        m_collision->SetTag(targetPin->GetObject(), ColliderTag::Platform);

        x += holeW;

        float rightW = TILE * 2.0f;
        AddPlatform(FLOOR_TEX, x + rightW * 0.5f, HIGH_Y, rightW, H);
        x += rightW;
    }

    //========================
    // 6 (敵の例 + マーカー登録)
    //========================
    {
        float sectionW = 1200.0f;
        AddPlatform(FLOOR_TEX, x + sectionW * 0.5f, HIGH_Y, sectionW, H);

        // Rippa
        Rippa* rippa = new Rippa(Rippa::Type::Wandering);
        rippa->Init("asset/Field/rippa.png", x + 700, HIGH_Y + 360, 140, 150);
        rippa->SetCollisionManager(m_collision);
        m_enemies.push_back(rippa);
        RegisterEnemyMarker(rippa, "asset/Field/フェルトワッペン リッパー.png", true);

        // WingRippa
        WingRippa* wirippa = new WingRippa(WingRippa::Type::ZigZag);
        wirippa->Init("asset/Field/Wing_Rippa.png", x + 900, HIGH_Y + 550, 150, 150);
        wirippa->SetCollisionManager(m_collision);
        m_enemies.push_back(wirippa);
        RegisterEnemyMarker(wirippa, "asset/Field/フェルトワッペン 羽リッパー.png",true);

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
            BlockPin* m_targetPin2 = new BlockPin;
            m_targetPin2->Init("asset/Field/block.png", x + 580.0, -433.0f, 360, 65);
            m_targetPin2->SetCollisionManager(m_collision);
            m_targetPin2->SetcanRollPin(true);
            m_targetPin2->SetcanDecorate(false);
            m_pins.push_back(m_targetPin2);
            m_collision->AddStatic(m_targetPin2->GetObject());
            m_targetPin2->SetForceGround(true);
            m_collision->SetTag(m_targetPin2->GetObject(), ColliderTag::Platform);

            RemoteWindPin* m_hook = new RemoteWindPin;
            m_hook->Init("asset/Field/PinRemote.png", x - 50, 150, 35, 35);
            m_hook->SetCollisionManager(m_collision);
            m_hook->SetcanRollPin(false);
            m_hook->SetTarget(m_targetPin2);

            //ピンの種類のStateを"RemoteDeco(飾れる)"に
            m_hook->SetPinKind(PinKind::RemoteDeco);

            // ガイド
            m_hook->AddGuide({ x + 170, -433.0, 0 });

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

        float w1 = TILE * 6.0f;
        Pin* JumpPin = new Pin;

        JumpPin->Init("asset/Field/PinJump.png", x - 280, -130, 35, 35);
        JumpPin->SetCollisionManager(m_collision);
        m_pins.push_back(JumpPin);
        JumpPin->SetcanRollPin(false);
        JumpPin->SetcanDecorate(false);

        //ピンの種類のStateを"Jump(飾れる)"に
        JumpPin->SetPinKind(PinKind::Jump);

        BlockPin* m_targetPin;
        m_targetPin = AddPullPin(x - 70, -418.0f, true);

        //m_collision->AddStatic(m_targetPin->GetObject());
        m_targetPin->SetForceGround(true);
        m_targetPin->SetLimitPos(x , x-150, -418.0f, -418.0f);
        m_targetPin->SetMoveAxis(BlockPin::MoveAxis::Horizontal);
        m_collision->SetTag(m_targetPin->GetObject(), ColliderTag::Platform);


        AddPlatform("asset/Field/block.png", x + w1 * 0.45f, -700, w1, H);
        x += w1;
    }

    //========================
    // 11 (GoalPinの例) ※GoalはGoalPin所有想定
    //========================
    {
        const float platformCenterX = x;
        const float platformTopY = LOW_Y + (H * 0.5f);

        const float pinW = 35.0f;
        const float pinH = 35.0f;

        GoalPin* goalPin = new GoalPin();
        goalPin->Init(
            "asset/Field/PinDeco.png",                 // Pin本体の見た目（好きなのでOK）
            platformCenterX,
            platformTopY + (pinH * 0.5f),              // 足場の上面 + Pin半分
            pinW,
            pinH
        );
        goalPin->SetCollisionManager(m_collision);

        m_pins.push_back(goalPin);
        m_collision->SetTag(goalPin->GetObject(), ColliderTag::Pin);
        m_goal.push_back(goalPin->GetGoal());

    }

    // Draw list
    BuildDrawList();

    // HP init
    maxHP = m_player.GetMaxHP();
    currentHP = m_player.GetHP();
}

//==================================================
// Update (Stage1風に整理 + Stage2カメラオフセット + marker対応)
//==================================================
void TutorialStage2::Update()
{
    const float dt = 1.0f / 60.0f;

    //==================================================
    // (A) Camera (Stage2: offset)
    //==================================================
    {
        const DirectX::XMFLOAT3 playerPos = m_player.GetObject()->GetPos();

        // トリガー例
        if (playerPos.x > 6500.0f)
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
        {
            g_cameraPos = { g_cameraPos.x, -200.0f, g_cameraPos.z };
        }
    }

    //==================================================
    // (B) Player Update + Fall death
    //==================================================
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


    //==================================================
    // (D) SpawnQueue flush
    //==================================================
    //if (!m_spawnQueue.empty())
    //{
    //    for (size_t i = 0; i < m_spawnQueue.size(); ++i)
    //        m_enemies.push_back(m_spawnQueue[i]);
    //    m_spawnQueue.clear();
    //}

    //==================================================
    // (E) Pins Update
    //==================================================
    for (size_t i = 0; i < m_pins.size(); ++i)
    {
        Pin* pin = m_pins[i];
        if (!pin) continue;

        pin->Update(dt);

        if (BlockPin* blockPin = dynamic_cast<BlockPin*>(pin))
            m_collision->GetAABB(blockPin->GetObject());
    }

    //==================================================
    // (F) Collision solve
    //==================================================
    m_collision->CheckAll();

    //==================================================
    // (G) Tutorials (checkpoint)
    //==================================================
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

    //==================================================
    // (H) Goals
    //==================================================
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

    //==================================================
    // (I)Enemy
    //==================================================
    for (auto& enemy : m_enemies) {
        if (!enemy) continue;

        enemy->Update(dt);

        //リッパー君の場合は敵同士の衝突をチェック
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

            //リッパー君の場合は敵同士の衝突をチェック
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
    //==================================================
    // (J) Remove dead enemies（ループ中erase禁止）
    //==================================================
    /*m_enemies.erase(
        std::remove_if(
            m_enemies.begin(),
            m_enemies.end(),
            [&](Enemy* e)
            {
                if (!e) return true;

                if (e->IsDead())
                {
                    m_collision->Remove(e->GetObject());
                    e->UnInit();
                    delete e;
                    return true;
                }
                return false;
            }),
        m_enemies.end()
    );*/

    //==================================================
    // (K) HP UI
    //==================================================
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

    //==================================================
    // (L) Markers（Stage1互換：飾った瞬間の表示）
    //==================================================
    for (size_t i = 0; i < m_pinMarkers.size(); ++i)
    {
        m_pinMarkers[i].marker.Update(dt);

        if (!m_pinMarkers[i].pin) continue;

        const Pin::MarkerEvent ev = m_pinMarkers[i].pin->ConsumeMarkerEvent();
        if (ev == Pin::MarkerEvent::None) continue;

        m_pinMarkers[i].marker.ShowAtHead(m_pinMarkers[i].pin->GetObject(), 120.0f, 300.0f, 200.0f);
    }

    for (size_t i = 0; i < m_enemyMarkers.size(); ++i)
    {
        m_enemyMarkers[i].marker.Update(dt);

        if (m_enemyMarkers[i].enemy && m_enemyMarkers[i].enemy->ConsumeJustDecorated())
        {
            m_enemyMarkers[i].marker.ShowAtHead(m_enemyMarkers[i].enemy->GetObject(), 180.0f, 300.0f, 200.0f);
        }
    }

    //==================================================
    // (M) Dead flag
    //==================================================
    if (m_player.isDead())
        m_isPlayerDead = true;
}

//==================================================
// Draw
//==================================================
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

//==================================================
// UnInit
//==================================================
void TutorialStage2::UnInit()
{
    // Platform
    for (size_t i = 0; i < m_platforms.size(); ++i)
        m_platforms[i].UnInit();
    m_platforms.clear();

    // Enemy
    for (size_t i = 0; i < m_enemies.size(); ++i)
    {
        if (!m_enemies[i]) continue;
        m_enemies[i]->UnInit();
        delete m_enemies[i];
    }
    m_enemies.clear();

    // spawnQueue（残ってたら解放）
   /* for (size_t i = 0; i < m_spawnQueue.size(); ++i)
    {
        if (!m_spawnQueue[i]) continue;
        m_spawnQueue[i]->UnInit();
        delete m_spawnQueue[i];
    }
    m_spawnQueue.clear();*/

    // Background / Player
    m_background.UnInit();
    m_player.Uninit();

    // Pins
    for (size_t i = 0; i < m_pins.size(); ++i)
    {
        if (!m_pins[i]) continue;
        m_pins[i]->UnInit();
        delete m_pins[i];
    }
    m_pins.clear();

    // Tutorials
    for (size_t i = 0; i < m_tutorials.size(); ++i)
    {
        if (!m_tutorials[i]) continue;
        m_tutorials[i]->UnInit();
        delete m_tutorials[i];
    }
    m_tutorials.clear();

    // Goals（GoalPin所有想定なのでdeleteしない）
    m_goal.clear();

    // Markers
    m_pinMarkers.clear();
    m_enemyMarkers.clear();
    m_drawList.clear();

    delete m_collision;
    m_collision = nullptr;
}

//==================================================
// Respawn
//==================================================
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

#include "BossStage.h"
#include "Renderer.h"
#include "Enemy.h"
#include <algorithm>
#include "sound.h"

extern Sound* g_sound;
extern DirectX::XMFLOAT3 g_cameraPos;

static void RegisterEnemyCollision(CollisionManager* col, Enemy* e)
{
    if (!col || !e) return;

    Object* obj = e->GetObject();
    if (!obj) return;

    col->AddDynamic(obj);
    col->SetTag(obj, ColliderTag::Enemy);
}

void BossStage::RegisterEnemyMarker(Enemy* enemy, const char* markerTex, bool isMove)
{
    if (!enemy) return;

    m_enemyMarkers.emplace_back();
    DecoLinkEnemy& el = m_enemyMarkers.back();

    el.enemy = enemy;
    el.marker.Init(markerTex);
    el.marker.SetMoveMode(isMove);
    el.marker.Hide();
}

void BossStage::RegisterPinMarker(Pin* pin, const char* markerTex, bool isMove)
{
    if (!pin) return;

    m_pinMarkers.emplace_back();
    DecoLinkPin& link = m_pinMarkers.back();

    link.pin = pin;
    link.marker.Init(markerTex);
    link.marker.SetMoveMode(isMove);
    link.marker.Hide();
}

static Object MakeTopBand(float x, float platformTopY, float w)
{
    Object band;
    band.Init();
    band.AddTexture("asset/Field/fwfw.png");
    const float bandH = 30.0f;
    band.SetPos(x, platformTopY + (bandH * 0.5f) - 10.0f, 0.0f);
    band.SetSize(w, bandH, 0.0f);
    band.SetUVMode(Object::UVMode::Loop);
    const float repeatPer150 = w / 150.0f;
    band.SetUVLength(repeatPer150);
    band.SetUVScroll(0.0f);
    return band;
}

//------------------------------------------------------------
// ★重要：ここでは Collision に登録しない（vector再確保で Object* が死ぬのを避ける）
//------------------------------------------------------------
void BossStage::AddPlatformStatic(const char* tex, float x, float y, float w, float h)
{
    m_platforms.emplace_back();
    Platform& back = m_platforms.back();
    back.Init(tex, x, y, w, h);

    const float platformTopY = y + (h * 0.5f);
    m_fuwafuwas.emplace_back(MakeTopBand(x, platformTopY, w));
}

void BossStage::AddPlatformOneWay(const char* tex, float x, float y, float w, float h)
{
    m_oneWayPlatforms.emplace_back();
    Platform& back = m_oneWayPlatforms.back();
    back.Init(tex, x, y, w, h);

    const float platformTopY = y + (h * 0.5f);
    m_fuwafuwas.emplace_back(MakeTopBand(x, platformTopY, w));
}

//------------------------------------------------------------
// ★ここでまとめて Collision 登録（platform vector の配置が確定したあと）
//------------------------------------------------------------
void BossStage::FinalizeCollisionRegistrations()
{
    if (!m_collision) return;

    // Static Platforms
    for (size_t i = 0; i < m_platforms.size(); ++i)
    {
        Object* o = m_platforms[i].GetObject();
        if (!o) continue;
        m_collision->AddStatic(o);
        m_collision->SetTag(o, ColliderTag::Platform);
    }

    // OneWay Platforms（あなたの仕様に合わせる）
    // - もし oneWay も衝突判定に入れるなら AddStatic する
    // - 「上からだけ」判定は CollisionManager 側でタグやフラグで処理する想定
    for (size_t i = 0; i < m_oneWayPlatforms.size(); ++i)
    {
        Object* o = m_oneWayPlatforms[i].GetObject();
        if (!o) continue;

        // 必要ならこれも入れる：
        m_collision->AddStatic(o);

        m_collision->SetTag(o, ColliderTag::Platform);
    }
}

void BossStage::AddDecorPin(float x, float y, bool canDecorate)
{
    Pin* pin = new Pin();
    pin->Init("asset/Field/PinDeco.png", x, y, 35, 35);
    pin->SetCollisionManager(m_collision);

    pin->SetcanRollPin(true);
    pin->SetcanDecorate(canDecorate);
    pin->SetPinKind(PinKind::Deco);

    m_pins.push_back(pin);
    m_collision->SetTag(pin->GetObject(), ColliderTag::Pin);

    RegisterPinMarker(pin, "asset/Field/s_w.png", false);
}

BlockPin* BossStage::AddPullPin(float x, float y, bool canRollPin)
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

void BossStage::BuildDrawList()
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
    for (size_t i = 0; i < m_oneWayPlatforms.size(); ++i)
    {
        item.obj = m_oneWayPlatforms[i].GetObject();
        item.layer = DrawLayer::StageObject;
        m_drawList.push_back(item);
    }

    for (size_t i = 0; i < m_fuwafuwas.size(); ++i)
    {
        item.obj = &m_fuwafuwas[i];
        item.layer = DrawLayer::StageObject;
        m_drawList.push_back(item);
    }

    for (auto& em : m_enemyMarkers)
    {
        if (auto* o = em.marker.GetObject())
        {
            item.obj = o;
            item.layer = DrawLayer::Enemy;
            m_drawList.push_back(item);
        }

        if (em.marker.GetisMove() == false)
        {
            if (auto* o2 = em.marker.GetObject2())
            {
                item.obj = o2; item.layer = DrawLayer::Enemy; m_drawList.push_back(item);
            }
            if (auto* o3 = em.marker.GetObject3())
            {
                item.obj = o3; item.layer = DrawLayer::Enemy; m_drawList.push_back(item);
            }
        }
    }

    for (auto& pm : m_pinMarkers)
    {
        if (auto* o = pm.marker.GetObject())
        {
            item.obj = o;
            item.layer = DrawLayer::Enemy;
            m_drawList.push_back(item);
        }
    }

    for (size_t i = 0; i < m_enemies.size(); ++i)
    {
        if (!m_enemies[i]) continue;
        item.obj = m_enemies[i]->GetObject();
        item.layer = DrawLayer::Enemy;
        m_drawList.push_back(item);
    }

    for (size_t i = 0; i < m_pins.size(); ++i)
    {
        if (!m_pins[i]) continue;

        if (auto* o = m_pins[i]->GetMarkObject())
        {
            item.obj = o; item.layer = DrawLayer::Enemy; m_drawList.push_back(item);
        }
        if (auto* o = m_pins[i]->GetMarkObject2())
        {
            item.obj = o; item.layer = DrawLayer::Enemy; m_drawList.push_back(item);
        }
        if (auto* o = m_pins[i]->GetMarkObject3())
        {
            item.obj = o; item.layer = DrawLayer::Enemy; m_drawList.push_back(item);
        }

        item.obj = m_pins[i]->GetObject();
        item.layer = DrawLayer::StageObject;
        m_drawList.push_back(item);
    }

    for (size_t i = 0; i < m_goal.size(); ++i)
    {
        if (!m_goal[i]) continue;
        item.obj = m_goal[i]->GetObject();
        item.layer = DrawLayer::StageObject;
        m_drawList.push_back(item);
    }

    if (m_player.GetDecoratingEffectBack())
    {
        item.obj = m_player.GetDecoratingEffectBack();
        item.layer = DrawLayer::Enemy;
        m_drawList.push_back(item);
    }

    if (m_player.GetObject())
    {
        item.obj = m_player.GetObject();
        item.layer = DrawLayer::Player;
        m_drawList.push_back(item);
    }

    if (m_player.GetDecoratingEffectFront())
    {
        item.obj = m_player.GetDecoratingEffectFront();
        item.layer = DrawLayer::Player;
        m_drawList.push_back(item);
    }

    if (m_player.GetGuideLineObject())
    {
        item.obj = m_player.GetGuideLineObject();
        item.layer = DrawLayer::Player;
        m_drawList.push_back(item);
    }

    if (m_player.GetCircle())
    {
        item.obj = m_player.GetCircle();
        item.layer = DrawLayer::Player;
        m_drawList.push_back(item);
    }

    if (m_player.GetJustkiran())
    {
        item.obj = m_player.GetJustkiran();
        item.layer = DrawLayer::Player;
        m_drawList.push_back(item);
    }

    // ★一時寿命の疑い回避
    auto& ribbon = m_player.GetRibbon();
    auto  objs = ribbon.GetDrawObjects(); 
    for (auto* obj : objs)
    {
        if (!obj) continue;
        DrawItem it; it.obj = obj; it.layer = DrawLayer::Player;
        m_drawList.push_back(it);
    }

    item.obj = &m_HP_UI1; item.layer = DrawLayer::UI; m_drawList.push_back(item);
    item.obj = &m_HP_UI2; item.layer = DrawLayer::UI; m_drawList.push_back(item);
    item.obj = &m_HP_UI3; item.layer = DrawLayer::UI; m_drawList.push_back(item);
}

void BossStage::Init()
{
    g_cameraPos = { 0, 0, 0 };

    m_collision = new CollisionManager();

    m_player.Init();
    m_player.SetCollisionManager(m_collision);

    if (!m_hasCheckpoint)
    {
        m_currentCheckpoint = { 0, 150, 0 };
        m_hasCheckpoint = true;
    }
    m_player.GetObject()->SetPos(m_currentCheckpoint.x, m_currentCheckpoint.y, m_currentCheckpoint.z);

    m_isGoalReached = false;
    m_isPlayerDead = false;

    m_background.Init();
    m_background.AddTexture("asset/Field/stage.png");
    m_background.SetPos(6000.0f, 0.0f, 0.0f);
    m_background.SetSize(20000.0f, 2400.0f, 0.0f);
    m_background.SetUVMode(Object::UVMode::Loop);
    m_background.SetUVLength(5.0f);
    m_background.SetUVScroll(0.0f);

    m_HP_UI1.Init(); m_HP_UI1.AddTexture("asset/UI/cursor.png"); m_HP_UI1.SetSize(200, 200, 0);
    m_HP_UI2.Init(); m_HP_UI2.AddTexture("asset/UI/cursor.png"); m_HP_UI2.SetSize(200, 200, 0);
    m_HP_UI3.Init(); m_HP_UI3.AddTexture("asset/UI/cursor.png"); m_HP_UI3.SetSize(200, 200, 0);

    m_collision->AddDynamic(m_player.GetObject());
    m_collision->SetTag(m_player.GetObject(), ColliderTag::Player);

    m_platforms.clear();
    m_oneWayPlatforms.clear();
    m_enemies.clear();
    m_pins.clear();
    m_goal.clear();
    m_fuwafuwas.clear();
    m_pinMarkers.clear();
    m_enemyMarkers.clear();
    m_drawList.clear();

    // ★応急：再確保をほぼゼロにする
    m_platforms.reserve(4096);
    m_oneWayPlatforms.reserve(4096);
    m_fuwafuwas.reserve(4096);
    m_enemyMarkers.reserve(512);
    m_pinMarkers.reserve(1024);
    m_drawList.reserve(8192);
    m_pins.reserve(1024);
    m_enemies.reserve(256);
    m_goal.reserve(64);

    const float TILE = 150.0f;
    const float H = 600.0f;
    const float GROUND_Y = -600.0f;

    const char* FLOOR_TEX = "asset/Field/floar.png";
    const char* ONEWAY_TEX = "asset/Field/b_2.png";

    float x = -300.0f;

    {
        float w = TILE * 18.0f;
        AddPlatformStatic(FLOOR_TEX, x + w * 0.5f, GROUND_Y, w, H);
        x += w;

        AddPullPin(x - TILE * 4.0f, GROUND_Y + H + 120.0f, true);
        AddPullPin(x - TILE * 2.0f, GROUND_Y + H + 120.0f, false);

        Rippa* bossSub = new Rippa();
        bossSub->Init("asset/Field/rippa.png", x - TILE * 6.0f, GROUND_Y + H + 80.0f, 180.0f, 180.0f);
        m_enemies.push_back(bossSub);
        RegisterEnemyCollision(m_collision, bossSub);
        RegisterEnemyMarker(bossSub, "asset/Field/f_rippa.png", false);
    }

    {
        float w = TILE * 1.0f;
        AddPlatformStatic(FLOOR_TEX, x + w * 0.5f, GROUND_Y + H * 0.5f, w, H * 2.0f);
        x += TILE * 2.0f;
    }

    {
        float w = TILE * 30.0f;
        AddPlatformStatic(FLOOR_TEX, x + w * 0.5f, GROUND_Y, w, H);
        float arenaLeft = x;
        float arenaRight = x + w;
        x += w;

        const float baseX = arenaLeft + TILE * 6.0f;
        const float baseY = GROUND_Y + H + 120.0f;

        for (int col = 0; col < 3; ++col)
        {
            for (int row = 0; row < 3; ++row)
            {
                float px = baseX + col * TILE * 6.0f;
                float py = baseY + row * TILE * 2.2f;
                AddPlatformOneWay(ONEWAY_TEX, px, py, TILE * 4.0f, TILE * 0.6f);

                if (row == 1)        AddPullPin(px, py + TILE * 0.9f, true);
                if (row == 2 && col == 1) AddDecorPin(px, py + TILE * 0.9f, true);
            }
        }

        Rippa* boss = new Rippa();
        boss->Init("asset/Field/rippa.png", (arenaLeft + arenaRight) * 0.5f, GROUND_Y + H + 120.0f, 260.0f, 260.0f);
        m_enemies.push_back(boss);
        RegisterEnemyCollision(m_collision, boss);
        RegisterEnemyMarker(boss, "asset/Field/f_rippa.png", false);

        AddPlatformStatic(FLOOR_TEX, arenaRight + TILE * 2.0f, GROUND_Y + H * 0.9f, TILE * 1.0f, H * 2.5f);

        Goal* goal = new Goal();
        goal->Init("asset/Field/Goal.png", arenaRight + TILE * 4.0f, GROUND_Y + H * 2.6f, 220.0f, 220.0f);
        m_goal.push_back(goal);
    }

    // ★platform/oneWay の確保が終わったので、ここでまとめて Collision に登録
    FinalizeCollisionRegistrations();

    BuildDrawList();
}

void BossStage::Respawn()
{
    if (!m_hasCheckpoint) return;
    m_player.GetObject()->SetPos(m_currentCheckpoint.x, m_currentCheckpoint.y, m_currentCheckpoint.z);
    m_isPlayerDead = false;
}

void BossStage::SetRespawnPos(DirectX::XMFLOAT3 respos)
{
    m_currentCheckpoint = respos;
    m_hasCheckpoint = true;
}

void BossStage::Update()
{
    const float dt = 1.0f / 60.0f;
    if (!m_collision) return;

    maxHP = m_player.GetMaxHP();
    currentHP = m_player.GetHP();

    m_HP_UI1.SetColor(1, 1, 1, currentHP >= 1 ? 1.0f : 0.0f);
    m_HP_UI2.SetColor(1, 1, 1, currentHP >= 2 ? 1.0f : 0.0f);
    m_HP_UI3.SetColor(1, 1, 1, currentHP >= 3 ? 1.0f : 0.0f);

    m_HP_UI1.SetPos(g_cameraPos.x - 600.0f, g_cameraPos.y + 350.0f, 0);
    m_HP_UI2.SetPos(g_cameraPos.x - 500.0f, g_cameraPos.y + 350.0f, 0);
    m_HP_UI3.SetPos(g_cameraPos.x - 400.0f, g_cameraPos.y + 350.0f, 0);

    for (auto* e : m_enemies)
    {
        if (!e) continue;
        e->Update(dt);
    }

    for (auto* p : m_pins)
    {
        if (!p) continue;
        p->Update(dt);
    }

    m_player.Update(dt, m_platforms, m_enemies, m_pins);

    m_collision->CheckAll();

    for (auto& em : m_enemyMarkers)
    {
        if (!em.enemy) continue;
        em.marker.Update(dt, em.enemy->GetObject());
    }
    for (auto& pm : m_pinMarkers)
    {
        if (!pm.pin) continue;
        pm.marker.Update(dt, pm.pin->GetObject());
    }

    if (m_player.GetObject()->GetPos().y < m_fallDeadLineY)
    {
        m_isPlayerDead = true;
    }
    if (m_player.isDead())
    {
        m_isPlayerDead = true;
    }

    {
        DirectX::XMFLOAT3 p = m_player.GetObject()->GetPos();
        g_cameraPos.x = p.x;
        g_cameraPos.y = p.y;
        m_cameraNowPos = g_cameraPos;
    }

    m_background.SetUVScroll(m_background.GetUVScroll() + dt * 0.02f);

    BuildDrawList();
}

void BossStage::Draw()
{
    std::sort(m_drawList.begin(), m_drawList.end(),
        [](const DrawItem& a, const DrawItem& b) { return (int)a.layer < (int)b.layer; });

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

void BossStage::UnInit()
{
    for (auto* p : m_pins)
    {
        if (p) { p->UnInit(); delete p; }
    }
    m_pins.clear();

    for (auto* e : m_enemies)
    {
        if (e) { e->UnInit(); delete e; }
    }
    m_enemies.clear();

    for (auto* g : m_goal)
    {
        if (g) { g->UnInit(); delete g; }
    }
    m_goal.clear();

    m_platforms.clear();
    m_oneWayPlatforms.clear();
    m_fuwafuwas.clear();

    m_player.Uninit();

    m_background.UnInit();
    m_HP_UI1.UnInit();
    m_HP_UI2.UnInit();
    m_HP_UI3.UnInit();

    delete m_collision;
    m_collision = nullptr;
}
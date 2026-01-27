#include "Stage1.h"
#include "Renderer.h"
#include <algorithm>

// 既存と同様に外部にある想定
extern DirectX::XMFLOAT3 g_cameraPos;

void Stage1::AddPlatform(const char* tex, float x, float y, float w, float h)
{
    Platform p;
    p.Init(tex, x, y, w, h);
    m_platforms.push_back(p);

    Platform& back = m_platforms.back();
    m_collision->AddStatic(back.GetObject());
    m_collision->SetTag(back.GetObject(), ColliderTag::Platform);
}

void Stage1::AddDecorPin(float x, float y, bool canDecorate)
{
    Pin* pin = new Pin();
    pin->Init("asset/Field/PinDeco.png", x, y, 35, 35);
    pin->SetCollisionManager(m_collision);

    pin->SetcanRollPin(true);
    pin->SetcanDecorate(canDecorate);

    pin->SetPinKind(PinKind::Deco); //ピンの種類のStateを"Deco(飾れる)"に

   m_pins.push_back(pin);
}

BlockPin* Stage1::AddPullPin(float x, float y, bool canRollPin)
{
    BlockPin* pin = new BlockPin();
    pin->SetCollisionManager(m_collision);

    pin->SetcanRollPin(canRollPin);
    pin->SetcanDecorate(false);

    //巻取り(引き寄せ)の場合
    if (canRollPin)
    {
        pin->Init("asset/Field/PinRoll.png", x, y, 35, 35);
        pin->SetPinKind(PinKind::Roll);
    }
    //巻き取られの場合
    else if (!canRollPin)
    {
        pin->Init("asset/Field/PinPulled.png", x, y, 35, 35);
        pin->SetPinKind(PinKind::Pulled);
    }

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

void Stage1::BuildDrawList()
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

    item.obj = m_player.GetDecoratingEffectBack();
    item.layer = DrawLayer::Enemy;
    m_drawList.push_back(item);

    for (size_t i = 0; i < m_enemies.size(); ++i) {
        item.obj = m_enemies[i]->GetObject();
        item.layer = DrawLayer::Enemy;
        m_drawList.push_back(item);
    }

    for (size_t i = 0; i < m_pins.size(); ++i) {
        item.obj = m_pins[i]->GetMarkObject();
        item.layer = DrawLayer::BackObject;
        m_drawList.push_back(item);

        item.obj = m_pins[i]->GetObject();
        item.layer = DrawLayer::StageObject;
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

    item.obj = m_player.GetDecoratingEffectFront();
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
void Stage1::Init()
{
    g_cameraPos = { 0, 0, 0 };

    m_collision = new CollisionManager();

    m_player.Init();
    m_player.SetCollisionManager(m_collision);
    m_player.GetObject()->SetPos(5000, 150, 0);

    m_currentCheckpoint = { 0, 150, 0 };
    m_hasCheckpoint = true;

    m_background.Init();
    m_background.AddTexture("asset/Field/ステージ背景.png");
    m_background.SetPos(7200.0f, 0.0f, 0.0f);      // 横14400の中心に置くなら 7200
    m_background.SetSize(23000.0f, 2100.0f, 0.0f);

    // ★ここがUV繰り返し
    m_background.SetUVMode(Object::UVMode::Loop);

    // 横だけ10回繰り返したいなら「U方向の長さ」を10にする
    m_background.SetUVLength(5.0f);

    // スクロールしないなら0で固定
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

    m_platforms.clear();
    m_platforms.reserve(32);

    //1
    {
        float w = TILE * 10.0f;
        AddPlatform("asset/Field/床ブロック.png", x + w * 0.5f, LOW_Y, w, H);

        AddDecorPin(x + TILE * 10.0f + 0.85f * TILE, -317.0f, true);

        Tutorial* tutorial1 = new Tutorial();
        tutorial1->Init(
            "asset/Field/Boad.png",
            1000, -230, 200, 200,
            Tutorial::Type::Move,
            true,   //チュートリアル表示ON
            true    //チェックポイントON
        );
        tutorial1->InitTutorialImage(
            "asset/Field/Tutorial_PinDeco.png",
            1000, 500,   //表示サイズ
            300, 350      //看板からのオフセット
        );
        tutorial1->SetRespawnPosition(1000, -150, 0);
        m_tutorials.push_back(tutorial1);

        x += w;

        x += TILE * 1.7f;

    }

    //2
    {
        float w1 = TILE * 6.0f;
        AddPlatform("asset/Field/床ブロック.png", x + w1 * 0.5f, LOW_Y, w1, H);
        x += w1;

        float gap = TILE * 3.0f;

        AddDecorPin(x + 75, -317.0f, true);
        AddDecorPin(x + gap * 0.5f, -317.0f, true);
        AddDecorPin(x + (TILE * 2.5f), -317.0f, true);


        x += gap;

        float w2 = TILE * 5.0f;
        AddPlatform("asset/Field/床ブロック.png", x + w2 * 0.5f, LOW_Y, w2, H);

        Tutorial* tutorial1 = new Tutorial();
        tutorial1->Init(
            "asset/Field/Boad.png",
            2200, -230, 200, 200,
            Tutorial::Type::Move,
            true,   //チュートリアル表示ON
            true    //チェックポイントON
        );
        tutorial1->InitTutorialImage(
            "asset/Ui/title.png",
            1000, 500,   //表示サイズ
            300, 450      //看板からのオフセット
        );
        tutorial1->SetRespawnPosition(2200, -150, 0);
        m_tutorials.push_back(tutorial1);

        x += w2;
    }

    //3
    {
        float w1 = TILE * 1.0f;
        AddPlatform("asset/Field/床ブロック.png", x + w1 * 0.5f, LOW_Y, w1, H);
        x += w1;

        float gap = TILE * 5.0f;

        AddDecorPin(x + TILE * 0.5f, -317.0f, true);
        AddDecorPin(x + TILE * 2.3f, -317.0f, true);
        AddDecorPin(x + TILE * 4.3f, -317.0f, true);

        x += gap;

        float w2 = TILE * 6.0f;
        AddPlatform("asset/Field/床ブロック.png", x + w2 * 0.5f + 125, LOW_Y, w2, H);
        x += w2;
    }

    //4
    {
        float w1 = TILE * 10.0f;
        AddPlatform("asset/Field/床ブロック.png", x + w1 * 0.5f, LOW_Y, w1, H);

        Tutorial* tutorial1 = new Tutorial();
        tutorial1->Init(
            "asset/Field/Boad.png",
            (x + w1 * 0.5f) - 800, -230, 200, 200,
            Tutorial::Type::Move,
            true,   //チュートリアル表示ON
            true    //チェックポイントON
        );
        tutorial1->InitTutorialImage(
            "asset/Field/Tutorial_NeedleFloor.png",
            1000, 500,   //表示サイズ
            300, 450      //看板からのオフセット
        );
        tutorial1->SetRespawnPosition((x + w1 * 0.5f) - 500, -150, 0);
        m_tutorials.push_back(tutorial1);

        NeedleFloor* nf = new NeedleFloor();

        nf->Init("asset/Field/needle_floor.png", x + w1 * 0.5f, LOW_Y + 340, 250, 100);

        // CollisionManager をセット（将来のリボン判定用）
        nf->SetCollisionManager(m_collision);

        m_enemies.push_back(nf);

        x += w1;
    }

    //5
    {
        float w1 = TILE * 5.0f;
        AddPlatform("asset/Field/床ブロック.png", x + w1 * 0.5f, LOW_Y, w1, H);

        Tutorial* tutorial1 = new Tutorial();
        tutorial1->Init(
            "asset/Field/Boad.png",
            (x - 500), -230, 200, 200,
            Tutorial::Type::Move,
            true,   //チュートリアル表示ON
            false    //チェックポイントOFF
        );
        tutorial1->InitTutorialImage(
            "asset/Field/Tutorial_Rippa.png",
            1000, 500,   //表示サイズ
            300, 450      //看板からのオフセット
        );
        m_tutorials.push_back(tutorial1);

        Rippa* rippa = new Rippa(Rippa::Type::Normal);
        rippa->Init("asset/Field/rippa.png", x + w1 * 0.5f, LOW_Y + 350, 150, 150);
        rippa->SetCollisionManager(m_collision);

        m_enemies.push_back(rippa);   
        x += w1;
    }


    //6
    {
        float w1 = TILE * 5.0f;
        Rippa* rippa_2 = new Rippa(Rippa::Type::Wandering);
        rippa_2->Init("asset/Field/rippa.png", x , LOW_Y + 350, 150, 150);
        rippa_2->SetCollisionManager(m_collision);

        rippa_2->SetTurnInterval(2.5f);

        m_enemies.push_back(rippa_2);

       
    }
    //7
    {
        float w1 = TILE * 1.0f;
        AddPlatform("asset/Field/床ブロック.png", x + w1 * 0.5f, LOW_Y, w1, H);

        AddDecorPin(x + w1 * 0.5 + 300, -317.0f, true);

        x += w1 + 450;
    }

    //8
    {
        float w1 = TILE * 10.0f;
        AddPlatform("asset/Field/床ブロック.png", x + w1 * 0.5f, LOW_Y, w1, H);

        Rippa* rippa = new Rippa(Rippa::Type::Normal);
        rippa->Init("asset/Field/rippa.png", x + w1 * 0.5f, LOW_Y + 350, 150, 150);
        rippa->SetCollisionManager(m_collision);

        m_enemies.push_back(rippa);

        Rippa* rippa2 = new Rippa(Rippa::Type::Normal);
        rippa2->Init("asset/Field/rippa.png", x + w1, LOW_Y + 350, 150, 150);
        rippa2->SetCollisionManager(m_collision);

        m_enemies.push_back(rippa2);

        x += w1;
    }

    //9
    {
        float w1 = TILE * 6.0f;

        Pin* JumpPin = new Pin;

        JumpPin->Init("asset/Field/PinJump.png", x + w1 *0.5 - TILE * 0.5, LOW_Y +600, 35, 35);
        JumpPin->SetCollisionManager(m_collision);
        m_pins.push_back(JumpPin);
        JumpPin->SetcanRollPin(false);
        JumpPin->SetcanDecorate(false);

        //ピンの種類のStateを"Jump(飾れる)"に
        JumpPin->SetPinKind(PinKind::Jump);

        AddPlatform("asset/Field/床ブロック.png", x + w1, LOW_Y, w1, H);

        Tutorial* tutorial1 = new Tutorial();
        tutorial1->Init(
            "asset/Field/Boad.png",
            x - TILE * 2, -230, 200, 200,
            Tutorial::Type::Move,
            true,   //チュートリアル表示ON
            true    //チェックポイントON
        );
        tutorial1->InitTutorialImage(
            "asset/Ui/title.png",
            1000, 500,   //表示サイズ
            300, 450      //看板からのオフセット
        );
        tutorial1->SetRespawnPosition(x - TILE * 2, -150, 0);
        m_tutorials.push_back(tutorial1);
        x += w1;
    }

    BuildDrawList();

    //HP系初期値
    maxHP = m_player.GetMaxHP();
    currentHP = m_player.GetHP();
}

void Stage1::Update()
{
    float dt = 1.0f / 60.0f;

    m_camera.Update(m_player.GetObject()->GetPos());

    m_cameraNowPos = m_player.GetObject()->GetPos();

    if (m_cameraNowPos.y < -500) {
        g_cameraPos = { g_cameraPos.x , -200, g_cameraPos.z };

    }

    // プレイヤー更新
    m_player.Update(dt, m_platforms, m_enemies, m_pins);

    const DirectX::XMFLOAT3 p = m_player.GetObject()->GetPos();
    if (p.y < m_fallDeadLineY)
    {
        DirectX::XMFLOAT2 dummyDir = { 0.0f, -1.0f };
        m_player.TakeDamage(1, dummyDir);

        Respawn();
    }

    // 敵
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
            }
            else if(needleflr->GetState() != NeedleFloor::State::Decorated)
            {
                m_player.TakeDamage(1, knockbackDir);
            }

            
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

void Stage1::Draw()
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

}

void Stage1::UnInit()
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

    for (auto& tutorial : m_tutorials)
    {
        tutorial->UnInit();
        delete tutorial;
    }

    delete m_collision;
    m_collision = nullptr;
}

void Stage1::Respawn()
{
    if (!m_hasCheckpoint)
        return;

    m_player.SetInvincible(false);

    //チェックポイント位置にプレイヤーを配置
    m_player.GetObject()->SetPos(
        m_currentCheckpoint.x,
        m_currentCheckpoint.y,
        m_currentCheckpoint.z
    );

    //死亡フラグをリセット
    m_isPlayerDead = false;

}

void Stage1::SetResoawnPos(DirectX::XMFLOAT3 respawnpos)
{
    m_currentCheckpoint = respawnpos;
}
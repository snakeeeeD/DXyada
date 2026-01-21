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
    pin->Init("asset/Field/Pin.png", x, y, 35, 35);
    pin->SetCollisionManager(m_collision);

    pin->SetcanRollPin(true);
    pin->SetcanDecorate(canDecorate);

    m_pins.push_back(pin);
}

BlockPin* Stage1::AddPullPin(float x, float y, bool canRollPin)
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


void Stage1::BuildDrawList()
{
    m_drawList.clear();

<<<<<<< Updated upstream

    Platform* plat1 = new Platform(); plat1->Init("asset/Field/block.png", 0, -400, 100, 50);     //手前床
    Platform* plat2 = new Platform(); plat2->Init("asset/Field/block.png", -200, -200, 1000, 50);   //主人公が乗っている床
    Platform* plat3 = new Platform(); plat3->Init("asset/Field/block.png", 1000, -150, 1500, 50); //ナガ床

    Platform* plat4 = new Platform(); plat4->Init("asset/Field/block.png", 400, -100, 100, 50); //左壁
    Platform* plat5 = new Platform(); plat5->Init("asset/Field/block.png", 1000, -100, 100, 50);  //右壁

    Platform* plat6 = new Platform(); plat6->Init("asset/Field/block.png", 1000, 100, 500, 50);  //リッパー崖判定の床

    Platform* plat7 = new Platform(); plat7->Init("asset/Field/block.png", 1000, -800, 1500, 50);  //下ナガ床
    Platform* plat8 = new Platform(); plat8->Init("asset/Field/block.png", -1000, 200, 100, 50);  //羽リッパーの壁反転用

    //エネミー1 (壁反転)
    {
        Rippa* rippa = new Rippa(Rippa::Type::Normal);
        rippa->Init("asset/Field/rippa.png", 800, -100, 150, 150);
        rippa->SetCollisionManager(m_collision);
        m_enemies.push_back(rippa);
    }

    //エネミー2 (崖反転)
    {
        Rippa* rippa_2 = new Rippa(Rippa::Type::Normal);
        rippa_2->Init("asset/Field/rippa.png", 1000, 150, 150, 150);
        rippa_2->SetCollisionManager(m_collision);

        m_enemies.push_back(rippa_2);
    }

    //エネミー3 (徘徊)
    {
        Rippa* rippa_3 = new Rippa(Rippa::Type::Wandering);
        rippa_3->Init("asset/Field/rippa.png", 1000, -700, 150, 150);
        rippa_3->SetCollisionManager(m_collision);

        rippa_3->SetTurnInterval(2.5f); //個々の値変更で自由に変えられる

        m_enemies.push_back(rippa_3);
    }

    //羽リッパー (通常)
    {
		WingRippa* wingrippa_1 = new WingRippa(WingRippa::Type::ZigZag);
		wingrippa_1->Init("asset/Field/Wing_Rippa.png", -500, 400, 150, 150);
		wingrippa_1->SetCollisionManager(m_collision);
        wingrippa_1->Init("asset/Field/Wing_Rippa.png", 100, 400, 100, 100);
        wingrippa_1->SetCollisionManager(m_collision);

        m_enemies.push_back(wingrippa_1);

    }

    //羽リッパー (通常・壁反転)
    {
        WingRippa* wingrippa_1 = new WingRippa(WingRippa::Type::ZigZag);
        wingrippa_1->Init("asset/Field/Wing_Rippa.png", -500, 200, 150, 150);
        wingrippa_1->SetCollisionManager(m_collision);

        m_enemies.push_back(wingrippa_1);

    }

    //羽リッパー (徘徊)
    {
        WingRippa* wingrippa_2 = new WingRippa(WingRippa::Type::Wandering_Circle);
        wingrippa_2->Init("asset/Field/Wing_Rippa.png", 1000, 500, 150, 150);
        wingrippa_2->SetCollisionManager(m_collision);

        m_enemies.push_back(wingrippa_2);

    }


    //ハリ床
    {
        NeedleFloor* nf = new NeedleFloor();

        // 床の高さに合わせて y=-150 に配置
        nf->Init("asset/Field/needle_floor.png", 0, -300, 150, 50);

        // CollisionManager をセット（将来のリボン判定用）
        nf->SetCollisionManager(m_collision);

        m_enemies.push_back(nf);
    }

    //はりやまさん
    {
        Hariyama* hariyama = new Hariyama();
		hariyama->Init("asset/Field/hariyama.png", 600, 600, 200, 200);
        hariyama->SetCollisionManager(m_collision);
        hariyama->SetEnemyList(&m_spawnQueue);

        hariyama->SetPlayer(&m_player);

		m_enemies.push_back(hariyama);

    }

    //ピン
    {
        Pin* pin = new Pin();
        pin->Init("asset/Field/Pin.png", 600, -300, 35, 35);
        pin->SetCollisionManager(m_collision);
        m_pins.push_back(pin);
        pin->SetcanRollPin(false);
        pin->SetcanDecorate(false);
    }

    m_BlockPinPos.x = 900.0f;
    m_BlockPinPos.y = -700.0f;

    Platform* plat9 = new Platform();
    plat9->Init("asset/Field/block.png", m_BlockPinPos.x + 50, m_BlockPinPos.y, 50, 50);
    //ピン付きブロック (地面付近のやつ)
    {

        BlockPin* blockpin = new BlockPin();
        blockpin->Init("asset/Field/Pin.png", m_BlockPinPos.x, m_BlockPinPos.y, 35, 35); //画像でき次第パス名を変更
        blockpin->SetCollisionManager(m_collision);
        m_pins.push_back(blockpin);
        blockpin->SetcanRollPin(true);

        // BlockPinのコリジョンを登録
        m_collision->SetTag(blockpin->GetObject(), ColliderTag::Pin);

        m_platforms.push_back(plat9);

        blockpin->SetTargetBlock(plat9);
    }
    

    {
        m_BlockPinPos.x = 0.0f;
        m_BlockPinPos.y = -700.0f;

        BlockPin* blockpin2 = new BlockPin();
        blockpin2->Init("asset/Field/Pin.png", m_BlockPinPos.x, m_BlockPinPos.y, 35, 35); //画像でき次第パス名を変更
        blockpin2->SetCollisionManager(m_collision);
        m_pins.push_back(blockpin2);
        blockpin2->SetcanRollPin(false);

        // BlockPinのコリジョンを登録
        //m_collision->AddStatic(blockpin2->GetObject());
        m_collision->SetTag(blockpin2->GetObject(), ColliderTag::Pin);
    }

    /*m_platforms = {
        plat1, plat2, plat3, plat4,
        plat5, plat6, plat7, plat8,
        plat9
    };*/

    m_platforms.push_back(plat1);
    m_platforms.push_back(plat2);
    m_platforms.push_back(plat3);
    m_platforms.push_back(plat4);
    m_platforms.push_back(plat5);
    m_platforms.push_back(plat6);
    m_platforms.push_back(plat7);
    m_platforms.push_back(plat8);
    

    for (auto& plat : m_platforms)
    {
        m_collision->AddStatic(plat->GetObject());
        m_collision->SetTag(plat->GetObject(), ColliderTag::Platform);
    }

    for (auto& enemy : m_enemies)
    {
        m_collision->AddMoved(enemy->GetObject());
        m_collision->SetTag(enemy->GetObject(), ColliderTag::Enemy);
    }

    //==========================================
    //HPのUI初期化
    //==========================================
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

    //////////////////////////////////////////////////////////////////描画設定のやり方//////////////////////////////////////////////////////////////////////
=======
>>>>>>> Stashed changes
    DrawItem item;

    item.obj = &m_background;
    item.layer = DrawLayer::Background;
    m_drawList.push_back(item);

<<<<<<< Updated upstream
    item.obj = &m_usiro;
    item.layer = DrawLayer::BackObject;
    m_drawList.push_back(item);

    for (size_t i = 0; i < m_platforms.size(); ++i)
    {
        item.obj = m_platforms[i]->GetObject();
=======
    for (size_t i = 0; i < m_platforms.size(); ++i) {
        item.obj = m_platforms[i].GetObject();
>>>>>>> Stashed changes
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

void Stage1::Init()
{
    g_cameraPos = { 0, 0, 0 };

    m_collision = new CollisionManager();

    m_player.Init();
    m_player.SetCollisionManager(m_collision);
    m_player.GetObject()->SetPos(6000, 150, 0);

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
            "asset/Ui/title.png",
            1000, 750,   //表示サイズ
            300, 450      //看板からのオフセット
        );
        tutorial1->SetRespawnPosition(1000, -150, 0);
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
        tutorial1->SetRespawnPosition(2500, -150, 0);
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
        float w1 = TILE * 10.0f;
        AddPlatform("asset/Field/block.png", x + w1 * 0.5f, LOW_Y, w1, H);
       
        Tutorial* tutorial1 = new Tutorial();
        tutorial1->Init(
            "asset/Field/Boad.png",
            (x + w1 * 0.5f) - 800, -230, 200, 200,
            Tutorial::Type::Move,
            true,   //チュートリアル表示ON
            true    //チェックポイントON
        );
        tutorial1->InitTutorialImage(
            "asset/Ui/title.png",
            1000, 750,   //表示サイズ
            300, 450      //看板からのオフセット
        );
        tutorial1->SetRespawnPosition((x + w1 * 0.5f) - 500, -150, 0);
        m_tutorials.push_back(tutorial1);

        NeedleFloor* nf = new NeedleFloor();

        nf->Init("asset/Field/needle_floor.png", x + w1 * 0.5f, LOW_Y + 320, 250, 50);
        
        // CollisionManager をセット（将来のリボン判定用）
        nf->SetCollisionManager(m_collision);
        
        m_enemies.push_back(nf);

        x += w1;
    }

    //5
    {
        float w1 = TILE * 5.0f;
        AddPlatform("asset/Field/block.png", x + w1 * 0.5f, LOW_Y, w1, H);

        Tutorial* tutorial1 = new Tutorial();
        tutorial1->Init(
            "asset/Field/Boad.png",
            (x - 500 ), -230, 200, 200,
            Tutorial::Type::Move,
            true,   //チュートリアル表示ON
            false    //チェックポイントON
        );
        tutorial1->InitTutorialImage(
            "asset/Ui/title.png",
            1000, 750,   //表示サイズ
            300, 450      //看板からのオフセット
        );
        m_tutorials.push_back(tutorial1);
        
        Rippa* rippa = new Rippa(Rippa::Type::Normal);
        rippa->Init("asset/Field/rippa.png", x + w1 * 0.5f, LOW_Y + 350, 150, 150);
        rippa->SetCollisionManager(m_collision);

        m_enemies.push_back(rippa);

        //6
        {
            Rippa* rippa_2 = new Rippa(Rippa::Type::Wandering);
            rippa_2->Init("asset/Field/rippa.png", x + w1 * 0.5f, LOW_Y + 350, 150, 150);
            rippa_2->SetCollisionManager(m_collision);

            rippa_2->SetTurnInterval(2.5f);

            auto rippaPos = rippa_2->GetObject()->GetPos();

            rippa->SetTurnRange(
                x - 625.0,
                rippaPos.x - 437.5
            );

            m_enemies.push_back(rippa_2);

            x += w1;
        }
    }

    //7
    {
        float w1 = TILE * 1.0f;
        AddPlatform("asset/Field/block.png", x + w1 * 0.5f, LOW_Y, w1, H);
       // x += w1;

        AddDecorPin(x + w1 * 0.5 + 300, -317.0f, true);

        x += 500;

        float w2 = TILE * 8.0f;
        AddPlatform("asset/Field/block.png", x + w2 * 0.5f, -900, w2, H);
        x += w2;
    }

    ////8
    //{
    //    x += 500;

    //    Pin* JumpPin = new Pin;

    //    JumpPin->Init("asset/Field/Pin.png", x - 250, -300, 35, 35);
    //    JumpPin->SetCollisionManager(m_collision);
    //    m_pins.push_back(JumpPin);
    //    JumpPin->SetcanRollPin(false);
    //    JumpPin->SetcanDecorate(false);

    //    float w1 = TILE * 6.0f;
    //    AddPlatform("asset/Field/block.png", x - 150 + w1 * 0.5f, -900, w1, H);
    //    x += w1;
    //}

    ////9
    //{
    //    x += 500;

    //    Pin* JumpPin = new Pin;

    //    JumpPin->Init("asset/Field/Pin.png", x - 500, -300, 35, 35);
    //    JumpPin->SetCollisionManager(m_collision);
    //    m_pins.push_back(JumpPin);
    //    JumpPin->SetcanRollPin(false);
    //    JumpPin->SetcanDecorate(false);

    //    BlockPin* m_targetPin;
    //    m_targetPin = AddPullPin(x - 200, -600.0f, true);

    //    m_collision->AddStatic(m_targetPin->GetObject());
    //    m_targetPin->SetForceGround(true);
    //    m_targetPin->SetLimitPos(x - 200, x - 300, -600.0f, -600.0f);
    //    m_targetPin->SetMoveAxis(BlockPin::MoveAxis::Horizontal);
    //    m_collision->SetTag(m_targetPin->GetObject(), ColliderTag::Platform);

    //    float w1 = TILE * 6.0f;
    //    AddPlatform("asset/Field/block.png", x - 150 + w1 * 0.5f, -900, w1, H);
    //    x += w1;
    //}

    ////10
    //{
    //    x += 1000;

    //    Pin* JumpPin = new Pin;

    //    JumpPin->Init("asset/Field/Pin.png", x - 900, -300, 35, 35);
    //    JumpPin->SetCollisionManager(m_collision);
    //    m_pins.push_back(JumpPin);
    //    JumpPin->SetcanRollPin(false);
    //    JumpPin->SetcanDecorate(false);

    //    {
    //        BlockPin* m_targetPin2 = new BlockPin;
    //        m_targetPin2->Init("asset/Field/Pin.png", x + 100.0, -600.0f, 350, 35);
    //        m_targetPin2->SetCollisionManager(m_collision);
    //        m_targetPin2->SetcanRollPin(true);
    //        m_targetPin2->SetcanDecorate(false);
    //        m_pins.push_back(m_targetPin2);
    //        m_collision->AddStatic(m_targetPin2->GetObject());
    //        m_targetPin2->SetForceGround(true);
    //        m_collision->SetTag(m_targetPin2->GetObject(), ColliderTag::Platform);

    //        RemoteWindPin* m_hook = new RemoteWindPin;
    //        m_hook->Init("asset/Field/Pin1.png", x - 1300, -400, 35, 35);
    //        m_hook->SetCollisionManager(m_collision);
    //        m_hook->SetcanRollPin(false);
    //        m_hook->SetTarget(m_targetPin2);

    //        // ガイド
    //        m_hook->AddGuide({ x - 500, -600, 0 });

    //        m_pins.push_back(m_hook);
    //        m_collision->SetTag(m_hook->GetObject(), ColliderTag::Pin);
    //    }

    //    float w1 = TILE * 3.0f;
    //    AddPlatform("asset/Field/block.png", x - 150 + w1 * 0.5f, -900, w1, H);
    //    x += w1;
    //}



    BuildDrawList();

    //HP系初期値
    maxHP = m_player.GetMaxHP();
    currentHP = m_player.GetHP();
}

void Stage1::Update()
{
    float dt = 1.0f / 60.0f;

    m_camera.Update(m_player.GetObject()->GetPos());

    // プレイヤー更新
    m_player.Update(dt, m_platforms, m_enemies, m_pins);

<<<<<<< Updated upstream
    // 敵更新
    for (auto& enemy : m_enemies)
    {

        //範囲判定
        float distance = 0.0f;
        bool inRange = m_player.IsEnemyInRange(enemy->GetObject()->GetPos(), distance);
        enemy->SetHighlight(inRange);

        enemy->Update(dt);

        //プレイヤーが透明でないとき
        if (!m_player.IsInvincible())
        {
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
    }

    for (auto* enemy : m_enemies)
    {
        enemy->Update(dt);
    }

    if (!m_spawnQueue.empty())
    {
        for (auto* newEnemy : m_spawnQueue)
        {
            m_enemies.push_back(newEnemy);
        }
        m_spawnQueue.clear(); //予約リストを空にする
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

=======
>>>>>>> Stashed changes
    const DirectX::XMFLOAT3 p = m_player.GetObject()->GetPos();
    if (p.y < m_fallDeadLineY)
    {
        DirectX::XMFLOAT2 dummyDir = { 0.0f, -1.0f };
        m_player.TakeDamage(1, dummyDir);

        Respawn();

        //m_isPlayerDead = true;
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

<<<<<<< Updated upstream
    for (auto* p : m_platforms)
    {
        p->Update(dt);
    }

    // コリジョン更新
    m_collision->CheckAll();

    if (m_player.isDead())
    {
=======
    if (m_player.isDead()) {
>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
void Stage1::UnInit() {
    for (auto& plat : m_platforms) plat->UnInit();
    for (auto& enemy : m_enemies) enemy->UnInit();
    for (auto& pin : m_pins) pin->UnInit();
=======
void Stage1::UnInit()
{
    // Platform
    for (auto& plat : m_platforms) plat.UnInit();
    m_platforms.clear();
>>>>>>> Stashed changes

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
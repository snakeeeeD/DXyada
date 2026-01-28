#include "Hariyama.h"
#include "Player.h"
#include "sound.h"   

extern Sound* g_sound;
void Hariyama::Init(const char* texture, float x, float y, float width, float height)
{
    Enemy::Init(texture, x, y, width, height);

    // 【事前ロード】飾り用の画像を "Decoration" という名前のアニメーションとして登録
    // これで画像がメモリに乗り、後で PlayAnimation("Decoration") するだけで切り替わります
    m_object.AddAnimation("Decoration", "asset/Field/Hariyama_Decorated.png", 1, 1, 0, 0, 0, 1.0f, false, false, 999);
}

void Hariyama::Update(float dt)
{

    // 凍結中は何もしない
    if (m_isFrozen)
    {
        m_object.Update(dt);
        return;
    }

    if (m_pPlayer)
    {

        DirectX::XMFLOAT3 playerPos = m_pPlayer->GetObject()->GetPos();
        DirectX::XMFLOAT3 myPos = m_object.GetPos();

        // 1. プレイヤーとの距離判定
        float diffX = playerPos.x - myPos.x;
        float dist = fabsf(diffX); // 絶対値で距離を測る

        if (dist <= ACTIVATION_RANGE)
        {
            // プレイヤーの方を向く
            m_direction = (diffX > 0) ? 1 : -1;

            // 2. 3秒タイマー
            m_shotTimer += dt;
            if (m_shotTimer >= SHOT_INTERVAL)
            {
                g_sound->Play(SOUND_LABEL_SE_Hariyamasan);
                Shoot();
                m_shotTimer = 0.0f; // リセット
            }
        }
        else
        {
            // 範囲外ならタイマーをリセット（画面に入ってから3秒後に撃つ場合）
            // そのままカウントを保持したいならここは消してもOK
            m_shotTimer = 0.0f;
        }
    }

    m_object.SetFlipX(m_direction > 0);
    m_object.Update(dt);
}

void Hariyama::Shoot()
{

    //リストに何も登録されていなければ何もしない
    if (m_pEnemyList == nullptr || m_pPlayer == nullptr)
    {
        return;
    }

    // プレイヤーの座標とはりやまの座標を取得
    auto playerPos = m_pPlayer->GetObject()->GetPos();
    auto myPos = m_object.GetPos();

    //針を生成
    {
        HariyamaNeedle* needle = new HariyamaNeedle(playerPos, myPos);

        //針を初期化
        auto pos = m_object.GetPos();
        needle->Init("asset/Field/Hariyama_Shoot.png", pos.x, pos.y, 180, 60);
        needle->SetCollisionManager(m_pCollision);
        m_pEnemyList->push_back(needle);
    }
}

HariyamaNeedle::HariyamaNeedle(DirectX::XMFLOAT3 targetPos, DirectX::XMFLOAT3 selfPos)
{
    // 1. 方向ベクトルを計算
    float diffX = targetPos.x - selfPos.x;
    float diffY = targetPos.y - selfPos.y;
    float length = sqrtf(diffX * diffX + diffY * diffY);

    if (length > 0) {
        m_moveVec.x = diffX / length;
        m_moveVec.y = diffY / length;

        // 2. 角度（ラジアン）を求める
        // atan2f は「右」を0とした角度を返す
        float angle = atan2f(m_moveVec.y, m_moveVec.x);

        // 3. 左側をプレイヤーに向けるため、180度(PI)回転させる
        // DirectXや多くのエンジンではラジアンを使用します
        const float PI = 3.14159265f;
        float degree = (angle + PI) * (180.0f / PI);

        // 4. オブジェクトに回転を適用
        // SetRotation(Z軸回転) などの関数名はお使いのライブラリに合わせてください
        m_object.SetAngle(degree);
    }
}

void HariyamaNeedle::Update(float dt)
{
    if (m_isFrozen) return;

    // 座標を移動させる（計算したベクトル方向にスピードをかける）
    auto pos = m_object.GetPos();
    pos.x += m_moveVec.x * m_speed * dt;
    pos.y += m_moveVec.y * m_speed * dt; // Y軸も動くようになる！

    m_object.SetPos(pos.x, pos.y, pos.z);

    // 以下の数字変更で消滅時間を変更できる
    m_lifeTimer += dt;
    if (m_lifeTimer > 5.0f)
    {
        m_enemystate = EnemyState::YouDied;
    }

    m_object.Update(dt);
}

void Hariyama::Disable(bool justdeco)
{

    Enemy::Disable(justdeco);

    if (justdeco)
    {

        m_object.PlayAnimation("Decoration");
    }
}
#include "Ribbon.h"
#include "Object.h"
#include <cmath>
#include "Enemy.h"

using namespace DirectX;

//====================================================
// コンストラクタ / デストラクタ
//====================================================
Ribbon::Ribbon()
{
}

Ribbon::~Ribbon()
{
    UnInit();
}

//====================================================
// 初期化
// ・リボン用 Object を必要数生成
//====================================================
void Ribbon::Init()
{
    m_segments.clear();

    // 最大節数分の Object を作る
    for (int i = 0; i < m_maxSegments; ++i)
    {
        Segment seg{};
        seg.obj = new Object();

        // 細長い板ポリとして初期化
        seg.obj->Init();
        seg.obj->AddTexture("asset/block.png"); // 仮テクスチャ
        seg.obj->SetSize(m_segmentLength, 6.0f, 0); // 細いリボン
        seg.obj->SetColor(1.0f, 0.2f, 0.7f, 1.0f);   // ピンク
        seg.obj->SetPos(0, 0, 0);

        seg.pos = { 0.0f, 0.0f };
        m_segments.push_back(seg);
    }

    m_state = State::Idle;
    m_time = 0.0f;
}

//====================================================
// プレイヤー位置セット
// Player::Update() から毎フレーム呼ぶ
//====================================================
void Ribbon::SetPlayerPos(const XMFLOAT2& pos)
{
    m_playerPos = pos;

    // 待機中は全部プレイヤーにくっつける
    if (m_state == State::Idle)
    {
        for (auto& seg : m_segments)
        {
            seg.pos = pos;
            seg.obj->SetPos(pos.x, pos.y, 0);
        }
    }
}

//====================================================
// 投げる処理
// dir : 投げる方向
//====================================================
void Ribbon::Throw(const XMFLOAT2& dir, float maxLength)
{
    if (m_state != State::Idle)
        return;

    float len = sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0.0f)
    {
        m_direction.x = dir.x / len;
        m_direction.y = dir.y / len;
    }

    m_maxLength = maxLength;
    m_currentLength = 0.0f;
    m_hasHit = false;
    m_hitEnemy = nullptr;
    m_state = State::Throwing;
}




//====================================================
// 戻す処理
//====================================================
void Ribbon::Return()
{
    if (m_state == State::Throwing)
    {
        m_state = State::Returning;
    }
}

//====================================================
// 更新処理
//====================================================
void Ribbon::Update(float deltaTime, const std::vector<Enemy*>& enemies)
{
    if (m_state == State::Idle)
        return;

    //----------------------------------
    // 伸びる
    //----------------------------------

  /*  if (m_state == State::Throwing)
    {
        m_currentLength += m_speed * deltaTime;

        if (m_currentLength >= m_maxLength)
        {
            m_currentLength = m_maxLength;

            // 先端位置を保存
            m_hitPos.x = m_playerPos.x + m_direction.x * m_currentLength;
            m_hitPos.y = m_playerPos.y + m_direction.y * m_currentLength;

            m_currentLength = m_maxLength;
            m_state = State::Returning;
        }
    }*/
    if (m_state == State::Throwing)
    {
        m_currentLength += m_speed * deltaTime;

        // 先端座標
        float tipX = m_playerPos.x + m_direction.x * m_currentLength;
        float tipY = m_playerPos.y + m_direction.y * m_currentLength;

        //壁ヒット判定
        if (m_collisionMgr)
        {
            CollisionManager::AABB tip;
            const float tipSize = 5.0f;

            tip.min = { tipX - tipSize, tipY - tipSize };
            tip.max = { tipX + tipSize, tipY + tipSize };

            if (m_collisionMgr->CheckHitStatic(tip))
            {
                // 壁に当たったら即戻す
                m_state = State::Returning;
                return;
            }
        }


        // 敵ヒット判定
        for (Enemy* enemy : enemies)
        {
            if (!enemy) continue;

            auto pos = enemy->GetObject()->GetPos();
            auto size = enemy->GetObject()->GetSize();

            float left = pos.x - size.x * 0.5f;
            float right = pos.x + size.x * 0.5f;
            float top = pos.y + size.y * 0.5f;
            float bottom = pos.y - size.y * 0.5f;

            if (tipX >= left && tipX <= right &&
                tipY >= bottom && tipY <= top)
            {
                m_hasHit = true;
                m_hitEnemy = enemy;
                m_hitPos = { tipX, tipY };
                m_state = State::Returning;
                break;
            }
        }

        if (m_currentLength >= m_maxLength)
        {
            m_currentLength = m_maxLength;
            m_state = State::Returning;
        }
    }
    //----------------------------------
    // 戻る
    //----------------------------------
    else if (m_state == State::Returning)
    {
        m_currentLength -= m_speed * deltaTime;

        if (m_currentLength <= 0.0f)
        {
            Reset();
            return;
        }
    }

    //----------------------------------
    // 描画更新（共通）
    //----------------------------------
    float px = m_playerPos.x;
    float py = m_playerPos.y;

    float centerX = px + m_direction.x * (m_currentLength * 0.5f);
    float centerY = py + m_direction.y * (m_currentLength * 0.5f);

    float angleRad = atan2(m_direction.y, m_direction.x);
    float angleDeg = angleRad * (180.0f / DirectX::XM_PI);

    auto& ribbon = m_segments[0];
    ribbon.obj->SetPos(centerX, centerY, 0);
    ribbon.obj->SetSize(m_currentLength, 6.0f, 0);
    ribbon.obj->SetPivot(0, 0, 0);
    ribbon.obj->SetAngle(angleDeg);
    ribbon.obj->SetColor(1, 1, 1, 1);
}


//====================================================
// リセット
// ・状態を待機に戻す
//====================================================
void Ribbon::Reset()
{
    m_state = State::Idle;
    m_currentLength = 0.0f;

    for (auto& seg : m_segments)
    {
        seg.obj->SetSize(0, 0, 0);
        seg.obj->SetColor(1, 1, 1, 0);
        seg.obj->SetPos(m_playerPos.x, m_playerPos.y, 0);
    }
}


//====================================================
// 描画用 Object 一覧取得
// Stage 側で DrawList に積むため
//====================================================
std::vector<Object*> Ribbon::GetDrawObjects() const
{
    std::vector<Object*> list;
    list.reserve(m_segments.size());

    for (const auto& seg : m_segments)
    {
        list.push_back(seg.obj);
    }

    return list;
}

//====================================================
// 解放処理
//====================================================
void Ribbon::UnInit()
{
    for (auto& seg : m_segments)
    {
        if (seg.obj)
        {
            seg.obj->UnInit();
            delete seg.obj;
            seg.obj = nullptr;
        }
    }

    m_segments.clear();
}
#include "Ribbon.h"
#include "Object.h"
#include <cmath>
#include <algorithm>
#include "Enemy.h"
#include "Pin.h"

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

    Segment seg{};
    seg.obj = new Object();

    // 細長い板ポリとして初期化
    seg.obj->Init();
    seg.obj->AddTexture("asset/Field/block.png"); // 仮テクスチャ
    seg.obj->SetSize(m_segmentLength, 6.0f, 0); // 細いリボン
    seg.obj->SetColor(1.0f, 0.2f, 0.7f, 0.0f);
    seg.obj->SetPos(0, 0, 0);

    seg.pos = { 0.0f, 0.0f };
    m_segments.push_back(seg);

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
    if (m_state == State::Throwing || m_state == State::Holding)
    {
        m_state = State::Returning;
    }
}

//====================================================
// 更新処理
//====================================================
void Ribbon::Update(float deltaTime, const std::vector<Enemy*>& enemies, std::vector<Pin*>& pins)
{
    if (m_state == State::Idle)
        return;
    DirectX::XMFLOAT2 targetPos{};
    bool hasTarget = false;

    //====================================================
    // 伸びる
    //====================================================
    if (m_state == State::Throwing)
    {
        if (!m_hasHit)
        {
            m_currentLength += m_speed * deltaTime;

            // 先端座標
            float tipX = m_playerPos.x + m_direction.x * m_currentLength;
            float tipY = m_playerPos.y + m_direction.y * m_currentLength;

            if (m_collisionMgr)
            {
                const int steps = 20; // 精度
                float stepLen = m_currentLength / steps;

                for (int i = 1; i <= steps; ++i)
                {
                    float checkX = m_playerPos.x + m_direction.x * stepLen * i;
                    float checkY = m_playerPos.y + m_direction.y * stepLen * i;

                    CollisionManager::AABB box;
                    const float size = 4.0f;
                    box.min = { checkX - size, checkY - size };
                    box.max = { checkX + size, checkY + size };

                    if (m_collisionMgr->CheckHitStatic(box))
                    {
                        //床に当たった位置で固定
                        m_currentLength = stepLen * (i - 1);

                        if (!m_isRTheld)
                        {
                            m_state = State::Returning;
                        }
                        break; //これ以上伸ばさない
                    }
                }
            }

            for (Enemy* enemy : enemies)
            {
                if (!enemy) continue;

                auto pos = enemy->GetObject()->GetPos();
                auto size = enemy->GetObject()->GetSize();

                float left = pos.x - size.x * 0.5f;
                float right = pos.x + size.x * 0.5f;
                float top = pos.y + size.y * 0.5f;
                float bottom = pos.y - size.y * 0.5f;

                // リボン先端が敵の範囲内に入ったか
                if (tipX >= left && tipX <= right &&
                    tipY >= bottom && tipY <= top)
                {
                    m_hasHit = true;
                    m_hitEnemy = enemy;
                    m_hitPos = { tipX, tipY };

                    m_breakLength = m_currentLength + 200.0f;

                    // RTを押していなければ戻す
                    if (m_isRTheld)
                    {
                        m_state = State::Holding;
                    }
                    else
                    {
                        m_state = State::Returning;
                    }

                    break;  // 最初に当たった敵で確定
                }
            }
            for (Pin* pin : pins)
            {
                if (!pin) continue;

                auto pos = pin->GetObject()->GetPos();
                auto size = pin->GetObject()->GetSize();

                float left = pos.x - size.x * 0.5f;
                float right = pos.x + size.x * 0.5f;
                float top = pos.y + size.y * 0.5f;
                float bottom = pos.y - size.y * 0.5f;

                if (tipX >= left && tipX <= right &&
                    tipY >= bottom && tipY <= top)
                {
                    if (!pin->CanBeHookedByRibbon())
                    {
                        m_state = State::Returning;
                        return;
                    }
                    m_hasHit = true;
                    m_hitPin = pin;
                    m_hitPos = { tipX, tipY };

                    m_breakLength = m_currentLength + 200.0f;

                    // RTを押していなければ戻す
                    if (m_isRTheld)
                    {
                        m_state = State::Holding;
                    }
                    else
                    {
                        m_state = State::Returning;
                    }
                    break;
                }
            }



            if (m_currentLength >= m_maxLength)
            {
                m_currentLength = m_maxLength;
                // RTを押していなければ戻す
                if (!m_isRTheld)
                {
                    m_state = State::Returning;
                }
            }
        }

    }
    //====================================================
    // RT長押し中
    //====================================================
    else if (m_state == State::Holding)
    {
        DirectX::XMFLOAT2 targetPos{};

        if (m_hitEnemy)
        {
            auto p = m_hitEnemy->GetObject()->GetPos();
            targetPos = { p.x, p.y };
        }
        else if (m_hitPin)
        {
            auto p = m_hitPin->GetObject()->GetPos();
            targetPos = { p.x, p.y };
        }

        float dx = targetPos.x - m_playerPos.x;
        float dy = targetPos.y - m_playerPos.y;
        float dist = sqrt(dx * dx + dy * dy);

        if (dist > 0.0f)
        {
            m_direction = { dx / dist, dy / dist };
        }

        m_currentLength = dist;

        // RT長押し中だけ余裕分を伸ばせる
        if (m_isRTheld)
        {
            m_currentLength += m_speed * deltaTime;
        }

        // 壁制限
        float wallLimit = CalcMaxReachByWall();
        m_currentLength = (std::min)(m_currentLength, wallLimit);

        // 自切
        if (m_currentLength >= m_breakLength)
        {
            m_state = State::Returning;
            return;
        }

        m_currentLength = (std::min)(m_currentLength, wallLimit);

        // 離したら戻る
        if (!m_isRTheld)
        {
            m_state = State::Returning;
        }
    }
    //====================================================
    // 戻る
    //====================================================
    else if (m_state == State::Returning)
    {
        m_currentLength -= m_speed * deltaTime;

        if (m_currentLength <= 0.0f)
        {
            Reset();
            return;
        }
    }
    CheckBodyHitWall();
    //====================================================
    // 描画更新（共通）
    //====================================================
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
    m_hasHit = false;
    m_hitEnemy = nullptr;
    m_hitPin = nullptr;

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


//====================================================
//床に当たるまでの最大到達距離を計算
//====================================================
float Ribbon::CalcMaxReachByWall() const
{
    if (!m_collisionMgr)
        return m_currentLength;

    const int steps = 30;
    float stepLen = m_currentLength / steps;

    for (int i = 1; i <= steps; ++i)
    {
        float checkX = m_playerPos.x + m_direction.x * stepLen * i;
        float checkY = m_playerPos.y + m_direction.y * stepLen * i;

        CollisionManager::AABB box;
        const float size = 4.0f;
        box.min = { checkX - size, checkY - size };
        box.max = { checkX + size, checkY + size };

        if (m_collisionMgr->CheckHitStatic(box))
        {
            return stepLen * (i - 1);
        }
    }

    return m_currentLength;
}

void Ribbon::CheckBodyHitWall()
{
    if (!m_collisionMgr)
        return;

    if (m_state != State::Throwing && m_state != State::Holding)
        return;

    const int steps = 25;
    float stepLen = m_currentLength / steps;

    for (int i = 1; i <= steps; ++i)
    {
        float checkX = m_playerPos.x + m_direction.x * stepLen * i;
        float checkY = m_playerPos.y + m_direction.y * stepLen * i;

        CollisionManager::AABB box;
        const float size = 4.0f;
        box.min = { checkX - size, checkY - size };
        box.max = { checkX + size, checkY + size };

        if (m_collisionMgr->CheckHitStatic(box))
        {
            //途中で床に当たったら戻す
            m_state = State::Returning;
            return;
        }
    }
}

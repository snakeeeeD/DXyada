#include "Ribbon.h"
#include "Object.h"
#include <cmath>

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
void Ribbon::Throw(const XMFLOAT2& dir)
{
    if (m_state == State::Throwing)
        return;

    // 方向ベクトル正規化
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    XMFLOAT2 d = dir;
    if (len > 0.0f)
    {
        d.x /= len;
        d.y /= len;
    }

    // 初速設定
    m_velocity = {
        d.x * m_speed,
        d.y * m_speed
    };

    m_time = 0.0f;
    m_state = State::Throwing;

    // 全節をプレイヤー位置にリセット
    for (auto& seg : m_segments)
    {
        seg.pos = m_playerPos;
        seg.obj->SetPos(m_playerPos.x, m_playerPos.y, 0);
    }
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
void Ribbon::Update(float deltaTime)
{
    if (m_state == State::Idle)
        return;

    m_time += deltaTime;

    //--------------------------------------------
    // 投げている最中
    //--------------------------------------------
    if (m_state == State::Throwing)
    {
        // 放物線の先端位置
        XMFLOAT2 head;
        head.x = m_playerPos.x + m_velocity.x * m_time;
        head.y = m_playerPos.y + m_velocity.y * m_time
            - 0.5f * m_gravity * m_time * m_time;

        // 後ろの節を前の節へ追従させる
        for (int i = (int)m_segments.size() - 1; i > 0; --i)
        {
            m_segments[i].pos = m_segments[i - 1].pos;
        }

        // 先端更新
        m_segments[0].pos = head;

        // Object に反映
        for (auto& seg : m_segments)
        {
            seg.obj->SetPos(seg.pos.x, seg.pos.y, 0);
        }

        //// 一定時間で戻り始める
        //if (m_time > 0.35f)
        //{
        //    m_state = State::Returning;
        //}
    }
    //--------------------------------------------
    // 戻り中
    //--------------------------------------------
    else if (m_state == State::Returning)
    {
        // 全節をプレイヤー方向へ補間
        bool finished = true;

        for (auto& seg : m_segments)
        {
            seg.pos.x += (m_playerPos.x - seg.pos.x) * 0.25f;
            seg.pos.y += (m_playerPos.y - seg.pos.y) * 0.25f;
            seg.obj->SetPos(seg.pos.x, seg.pos.y, 0);

            float dx = m_playerPos.x - seg.pos.x;
            float dy = m_playerPos.y - seg.pos.y;
            if (std::fabs(dx) > 2.0f || std::fabs(dy) > 2.0f)
            {
                finished = false;
            }
        }

        // 全部戻ったら待機
        if (finished)
        {
            Reset();
        }
    }
}

//====================================================
// リセット
// ・状態を待機に戻す
//====================================================
void Ribbon::Reset()
{
    m_state = State::Idle;
    m_time = 0.0f;

    // 全節をプレイヤー位置に戻す
    for (auto& seg : m_segments)
    {
        seg.pos = m_playerPos;
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
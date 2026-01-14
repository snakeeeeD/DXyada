#include "WingRippa.h"
#include <cmath>

WingRippa::WingRippa(Type type) : m_type(type) {}

void WingRippa::Update(float deltaTime)
{
    if (IsDead()) return;

    //LT押してるときは遅く
    if (m_isSlow)
    {
        m_walkspeed = 60.0f;
    }
    else
    {
        m_walkspeed = 120.0f;
    }

    if (!m_InitBase)
    {
        auto pos = m_object.GetPos();
        m_baseX = pos.x;
        m_baseY = pos.y;
        m_InitBase = true;
    }

    switch (m_type)
    {
    case Type::ZigZag:
        UpdateZigZag(deltaTime);
        break;

    case Type::Wandering_Circle:
        UpdateCircle(deltaTime);
        break;
    }
}

void WingRippa::UpdateZigZag(float dt)
{
	// ジグザグ移動の実装
	m_time += 1.0f / 240.0f; // フレームごとに時間を進める

	auto pos = m_object.GetPos();

    //左右移動
    pos.x += m_direction * m_walkspeed * dt;

	//上下移動
	pos.y += m_verticalDir * m_walkspeed * dt;

    // 上下移動の反転判定
    bool isTurnedVertical = false;

    if (pos.y > m_baseY + m_amplitude)
    {
        pos.y = m_baseY + m_amplitude;
        m_verticalDir = -1;     // 下向き
        isTurnedVertical = true;
    }
    else if (pos.y < m_baseY - m_amplitude)
    {
        pos.y = m_baseY - m_amplitude;
		m_verticalDir = 1;      // 上向き
		isTurnedVertical = true;
    }

    // 3. ジグザグ回数のカウントと左右反転
    if (isTurnedVertical)
    {
        m_zigzagCount++;

        // 指定回数（n回）ジグザグしたら左右の向きを反転
        if (m_zigzagCount >= m_maxZigzag)
        {
            m_direction *= -1; // 左右反転
            m_zigzagCount = 0; // カウントリセット
        }
    }

    m_object.SetFlipX(m_direction > 0);
    
	m_object.SetPos(pos.x, pos.y, pos.z);

	CheckWallCollision();
}

void WingRippa::UpdateCircle(float dt)
{
    auto oldPos = m_object.GetPos();
    m_time += dt;
    auto pos = m_object.GetPos();

    // 円軌道の計算
    // baseX/Y を中心に、半径 m_radius の円を描く
    pos.x = m_baseX + cosf(m_time * m_frequency) * m_radius;
    pos.y = m_baseY + sinf(m_time * m_frequency) * m_radius;

    m_object.SetFlipX(pos.x > oldPos.x);

    m_object.SetPos(pos.x, pos.y, pos.z);
}

void WingRippa::CheckWallCollision()
{
    CollisionManager::AABB wallSensor;

    auto pos = m_object.GetPos();
    auto size = m_object.GetSize();
    float halfW = size.x / 2.0f;

    // 羽リッパーは上下に動くので、yLow/yHigh は現在の pos.y を基準にする
    float sensorDist = 5.0f;
    float yLow = pos.y - 10.0f;
    float yHigh = pos.y + 10.0f;

    // 壁判定用の AABB 設定（崖判定は削除）
    if (m_direction > 0) // 右向き
    {
        wallSensor.min = { pos.x + halfW + 1.0f, yLow };
        wallSensor.max = { pos.x + halfW + 1.0f + sensorDist, yHigh };
    }
    else // 左向き
    {
        wallSensor.min = { pos.x - halfW - 1.0f - sensorDist, yLow };
        wallSensor.max = { pos.x - halfW - 1.0f, yHigh };
    }

    // 壁との接触確認
    bool isHitWall = m_pCollision->CheckHitStatic(wallSensor);

    if (isHitWall)
    {
        // 1. 左右反転
        m_direction *= -1;

        // 2. ジグザグ回数をリセット（壁に当たったら心機一転）
        m_zigzagCount = 0;

        // 3. 埋まり防止：反転した方向に少し戻す
        pos.x += m_direction * 2.0f;
        m_object.SetPos(pos.x, pos.y, pos.z);
    }
}
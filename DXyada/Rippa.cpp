#include "Rippa.h"

Rippa::Rippa(Type type) : m_type(type) {}

void Rippa::Update(float deltaTime)
{
    //LT押してるときは遅く
    if (m_isSlow)
    {
        m_walkspeed = 30.0f;
    }
    else
    {
        m_walkspeed = 60.0f;
    }

	auto pos = m_object.GetPos();

	// 1. 移動処理
	pos.x += m_walkspeed * m_direction * deltaTime;
	m_object.SetPos(pos.x, pos.y, pos.z);

    switch (m_type)
    {
        case Type::Normal:
            CheckCollision();
            break;

        case Type::Wandering:
            CheckWandering(deltaTime);
            break;
    }

    m_object.SetFlipX(m_direction > 0);

	m_object.Update(deltaTime);
}

void Rippa::CheckCollision()
{
    CollisionManager::AABB wallSensor;
    CollisionManager::AABB cliffSensor;

    auto pos = m_object.GetPos();
    auto size = m_object.GetSize();
    float halfW = size.x / 2.0f;
    float halfH = size.y / 2.0f; // 高さは halfH を使うのが一般的です

    float sensorDist = 5.0f;
    float yLow = pos.y - 10.0f;
    float yHigh = pos.y + 10.0f;

    float cliffCheckDist = 10.0f;
    float cliffYRange = 20.0f;

    // 足元(pos.y - halfH)を基準にする
    float footY = pos.y - halfH; // ここは halfH が正しいです
    float yBottomCliff = footY - 10.0f; // 足元より少し下（-60付近）

    if (m_direction > 0)  // 右向き時
    {
        wallSensor.min = { pos.x + halfW + 1.0f, yLow };
        wallSensor.max = { pos.x + halfW + 1.0f + sensorDist, yHigh };

        cliffSensor.min = { pos.x + halfW, yBottomCliff - cliffYRange };
        cliffSensor.max = { pos.x + halfW + cliffCheckDist, yBottomCliff };
    }
    else  // 左向き時
    {
        //halfH を halfW に変更し、xの計算を整理
        wallSensor.min = { pos.x - halfW - 1.0f - sensorDist, yLow };
        wallSensor.max = { pos.x - halfW - 1.0f, yHigh };

        //y座標の計算を右向き時と合わせる
        cliffSensor.min = { pos.x - halfW - cliffCheckDist, yBottomCliff - cliffYRange };
        cliffSensor.max = { pos.x - halfW, yBottomCliff };
    }

    bool isHitWall = m_pCollision->CheckHitStatic(wallSensor);
    bool isOnGround = m_pCollision->CheckHitStatic(cliffSensor);

    if (isHitWall || !isOnGround)
    {
        m_direction *= -1;

        // 座標を即座に更新して、次のフレームの二重判定を防ぐ
		if (m_direction > 0)
		{
			pos.x += 2.0f;
		}
		else 
		{ 
		    pos.x -= 2.0f; 
		}
    }

	// 移動反映
	pos.x += m_walkspeed * m_direction * deltaTime;

	// 座標セット
	m_object.SetPos(pos.x, pos.y, pos.z);

	m_object.Update(deltaTime);
}

void Rippa::CheckWandering(float deltaTime)
{
    m_turnTimer += deltaTime;

    // 設定した間隔（m_turnInterval）が来たら反転
    if (m_turnTimer >= m_turnInterval)
    {
        m_direction *= -1;
        m_turnTimer = 0.0f;
    }
}
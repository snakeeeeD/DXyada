#include "Rippa.h"

Rippa::Rippa(Type type) : m_type(type) {}

void Rippa::Init(const char* texture, float x, float y, float width, float height)
{
    Enemy::Init(texture, x, y, width, height);

    // 【事前ロード】飾り用の画像を "Decoration" という名前のアニメーションとして登録
    // これで画像がメモリに乗り、後で PlayAnimation("Decoration") するだけで切り替わります
    m_object.AddAnimation("Decoration", "asset/Field/Rippa_Decorated.png", 1, 1, 0, 0, 0, 1.0f, false, false, 999);
}

void Rippa::Update(float deltaTime)
{
    //LT押してるときは遅く
    if (m_isSlow)
    {
        m_walkspeed = 85.0f;
    }
    else
    {
        m_walkspeed = 175.0f;
    }

    auto pos = m_object.GetPos();

    if (m_useTurnRange)
    {
        if (pos.x < m_turnMinX)
            m_direction = 1;
        else if (pos.x > m_turnMaxX)
            m_direction = -1;
    }

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

void Rippa::SetTurnPos(float min, float max)
{
    auto pos = m_object.GetPos();

    if (pos.x < min)
    {
        m_direction = 1;
    }
    if (pos.x > max)
    {
        m_direction = -1;
    }

}

void Rippa::SetTurnRange(float min, float max)
{
    m_turnMinX = min;
    m_turnMaxX = max;
    m_useTurnRange = true;
}

void Rippa::CheckEnemyCollision(const std::vector<Enemy*>& enemies)
{
    if (m_type != Type::Normal)
        return;

    auto myPos = m_object.GetPos();
    auto mySize = m_object.GetSize();

    for (auto* enemy : enemies)
    {
        if (!enemy || enemy == this)  //自分自身はスキップ
            continue;

        auto enemyPos = enemy->GetObject()->GetPos();
        auto enemySize = enemy->GetObject()->GetSize();

        //AABBで衝突判定
        float myLeft = myPos.x - mySize.x * 0.5f;
        float myRight = myPos.x + mySize.x * 0.5f;
        float myTop = myPos.y + mySize.y * 0.5f;
        float myBottom = myPos.y - mySize.y * 0.5f;

        float enemyLeft = enemyPos.x - enemySize.x * 0.5f;
        float enemyRight = enemyPos.x + enemySize.x * 0.5f;
        float enemyTop = enemyPos.y + enemySize.y * 0.5f;
        float enemyBottom = enemyPos.y - enemySize.y * 0.5f;

        //衝突チェック
        if (myRight > enemyLeft && myLeft < enemyRight &&
            myTop > enemyBottom && myBottom < enemyTop)
        {
            //衝突したら反転
            m_direction *= -1;

            float pushDistance = 5.0f;
            myPos.x += m_direction * pushDistance;
            m_object.SetPos(myPos.x, myPos.y, myPos.z);

            break;  //衝突したら終了
        }
    }
}

void Rippa::Disable(bool justdeco)
{

    Enemy::Disable(justdeco);

    if (justdeco)
    {

        m_object.PlayAnimation("Decoration");
    }
}
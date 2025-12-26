#include "BlockPin.h"

BlockPin::BlockPin() : m_moveSpeed(0.0f){}

BlockPin::~BlockPin() {}

void BlockPin::Init(const char* texture, float x, float y, float width, float height)
{
	Pin::Init(texture, x, y, width, height);
	m_object.SetPos(x, y, 0.0f);
	m_object.SetSize(width, height, 0);
}

void BlockPin::Update(float dt)
{
	Pin::Update(dt);
}

void BlockPin::Draw()
{
	Pin::Draw();
}

void BlockPin::OnWindUp(const DirectX::XMFLOAT3& playerPos, float dt, float playerSpeed)
{
    ////リボンがかかっていないなら動かない
    //if (m_state != State::Decorated)
    //{
    //    return;
    //}

    DirectX::XMFLOAT3 currentPos = m_object.GetPos();

    // プレイヤーへの方向ベクトルを計算
    float dx = playerPos.x - currentPos.x;
    float dy = playerPos.y - currentPos.y;
    float distance = sqrt(dx * dx + dy * dy);

    const float stopDistance = 80.0f;

    // 距離が十分近い場合は移動しない
    if (distance <= stopDistance)
    {
        return;
    }

    // 正規化された方向ベクトル
    float dirX = dx / distance;
    float dirY = dy / distance;

    // 移動速度を設定
    m_moveSpeed = playerSpeed * 1.0f;

    // プレイヤーの方向へ移動（X軸とY軸両方）
    currentPos.x += dirX * m_moveSpeed * dt;
    currentPos.y += dirY * m_moveSpeed * dt;

    // 行き過ぎ防止（プレイヤー位置を超えないように）
    float newDx = playerPos.x - currentPos.x;
    float newDy = playerPos.y - currentPos.y;

    // 方向が反転したら、プレイヤー位置に固定
    if ((dx * newDx < 0) || (dy * newDy < 0))
    {
        currentPos.x = playerPos.x;
        currentPos.y = playerPos.y;
    }

    m_object.SetPos(currentPos.x, currentPos.y, currentPos.z);
}
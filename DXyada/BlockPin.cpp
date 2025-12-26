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
	//リボンがかかっていないなら動かない
	if (m_state != State::Decorated)
	{
		return;
	}

	DirectX::XMFLOAT3 currentPos = m_object.GetPos();

	m_moveSpeed = playerSpeed * 0.33f;

	//プレイヤーの方へ移動
	if (currentPos.x < playerPos.x)
	{
		currentPos.x += m_moveSpeed * dt;

		//行き過ぎ防止
		if (currentPos.x > playerPos.x)
		{
			currentPos.x = playerPos.x;
		}
	}
	else if (currentPos.x > playerPos.x)
	{
		currentPos.x -= m_moveSpeed * dt;

		// 行き過ぎ防止
		if (currentPos.x < playerPos.x)
		{
			currentPos.x = playerPos.x;
		}
	}

	m_object.SetPos(currentPos.x, currentPos.y, currentPos.z);
}
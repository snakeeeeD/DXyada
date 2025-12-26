#pragma once
#include "Pin.h"

class BlockPin : public Pin
{
private:
	//DirectX::XMFLOAT3 m_initialPos; //ミスしたときのリセット
	float m_moveSpeed;      //移動速度

public:
	BlockPin();
	virtual ~BlockPin();

	void Init
	(
		const char* texture, 
		float x, float y, 
		float width, 
		float height
	) override;

	void Update(float dt) override;

	virtual void Draw() override;

	void OnWindUp(const DirectX::XMFLOAT3& playerPos, float dt, float playerSpeed) override;
};
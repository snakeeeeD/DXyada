#pragma once
#include "Object.h"
#include "CollisionManager.h"

class Pin
{
public:
	enum class State
	{
		Normal,
		Decorated //リボンが巻かれている
	};

protected:
	Object m_object;
	State m_state = State::Normal;
	CollisionManager* m_pCollision = nullptr;

	bool m_canRollPin = false;

public:
	Pin() = default;
	~Pin() = default;

	virtual void Init();
	virtual void Init(const char* texture, float x, float y, float width, float height);
	virtual void Update(float dt);
	virtual void Draw();
	virtual void UnInit();

	//リボンが当たったときの処理
	virtual void OnHit();

	virtual void OnWindUp(const DirectX::XMFLOAT3& playerPos, float dt, float playerSpeed) {}

	//ゲッター
	State GetState() const { return m_state; }
	Object* GetObject() { return &m_object; }

	//引き寄せ、巻き取り判別用
	bool GetcanRollPin() const { return m_canRollPin; }
	void SetcanRollPin(bool canRoll) { m_canRollPin = canRoll; }

	void SetCollisionManager(CollisionManager* pCol) { m_pCollision = pCol; }
	
	void SetPos(float Pos_X, float Pos_Y);
};
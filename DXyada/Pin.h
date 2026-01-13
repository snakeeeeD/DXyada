#pragma once
#include "Object.h"
#include "CollisionManager.h"

class Pin
{
public:
	enum class State
	{
		Normal,
		Decorated,	//リボンが巻かれている
		Gorgeous	//豪華に巻かれている
	};

protected:
	Object m_object;
	State m_state = State::Normal;
	CollisionManager* m_pCollision = nullptr;

	bool m_forceGround = false;

	bool m_canRollPin = false;
	bool m_canDecorate = false;
	bool m_isPlatformRegistered = false;

private:
	bool m_enablePlatformRegisterOnDecorated = true;
	bool m_canBeGround = true;
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
	void SetState(State state);
	Object* GetObject() { return &m_object; }

	//引き寄せ、巻き取り判別用
	bool GetcanRollPin() const { return m_canRollPin; }
	void SetcanRollPin(bool canRoll) { m_canRollPin = canRoll; }

	//飾れるかの判別用
	bool GetcanDecorate() const { return m_canDecorate; }
	void SetcanDecorate(bool canDeco) { m_canDecorate = canDeco; }

	void SetCollisionManager(CollisionManager* pCol) { m_pCollision = pCol; }
	
	void SetPos(float Pos_X, float Pos_Y);

	void SetEnablePlatformRegisterOnDecorated(bool enable)
	{
		m_enablePlatformRegisterOnDecorated = enable;
	}

	bool CanBeGround() const { return m_canBeGround; }
	void SetCanBeGround(bool v) { m_canBeGround = v; }

	virtual bool IsGround() const
	{
		return m_forceGround || m_state == State::Decorated;
	}

	void SetForceGround(bool enable)
	{
		m_forceGround = enable;
	}

};
#pragma once
#include "Object.h"
#include "CollisionManager.h"

	enum class PinKind
	{
		Deco,
		RemoteDeco,
		Jump,
		Pulled,
		Roll,
		Normal
	};

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
	PinKind m_kind = PinKind::Deco;
	CollisionManager* m_pCollision = nullptr;

	bool m_forceGround = false;

	bool m_canRollPin = false;
	bool m_canDecorate = false;
	bool m_isPlatformRegistered = false;

private:
	bool m_enablePlatformRegisterOnDecorated = true;
	bool m_canBeGround = true;
	bool m_canBeHookedByRibbon = true;

	bool m_justDeco = false;

	Object m_mark;
	bool   m_markVisible = true;

	float  m_markOffsetX = 0.0f;
	float  m_markOffsetY = 20.0f;
	float  m_markW = 320.0f;
	float  m_markH = 320.0f;

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
	void SetState(State state , bool justdeco);
	Object* GetObject() { return &m_object; }
	//まーかー
	Object* GetMarkObject() { return &m_mark; }

	//引き寄せ、巻き取り判別用
	bool GetcanRollPin() const { return m_canRollPin; }
	void SetcanRollPin(bool canRoll) { m_canRollPin = canRoll; }

	//飾れるかの判別用
	bool GetcanDecorate() const { return m_canDecorate; }
	void SetcanDecorate(bool canDeco) { m_canDecorate = canDeco; }
	bool CanBeHookedByRibbon() const { return m_canBeHookedByRibbon; }

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

	PinKind GetPinKind() const { return m_kind; }
	void SetPinKind(PinKind pinkind) { m_kind = pinkind; }

};
#pragma once
#include "Enemy.h"

class NeedleFloor : public Enemy
{
public:
	enum class State
	{
		Spiky,
		Decorated
	};

private:
	State m_state = State::Spiky;

public:
	NeedleFloor() = default;
	~NeedleFloor() override = default;

	void Init(const char* texture, float x, float y, float width, float height);


	void Update(float deltaTime) override;

	//ƒŠƒ{ƒ“‚ª“–‚½‚Á‚½‚Æ‚«‚ÌŠÖ”(‰¼)
	void OnDecorated();

	//Œ»İ‚Ìó‘Ô
	State GetState() const { return m_state; }

	void Disable(bool justdeco);

};
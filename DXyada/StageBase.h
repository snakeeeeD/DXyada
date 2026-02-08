#pragma once
#include <vector>
#include "Platform.h"
#include "DecoMarker.h"

#include "Enemy.h"
#include "Rippa.h"
#include "WingRippa.h"
#include "Hariyama.h"
#include "NeedleFloor.h"

#include "Texture.h"
#include "Player.h"
#include "Camera2D.h"
#include "CollisionManager.h"
#include "Ribbon.h"
#include "Pin.h"
#include "BlockPin.h"
#include "RemoteWindPin.h"
#include "Tutorial.h"
#include "Goal.h"
#include "GoalPin.h"

enum class DrawLayer {
    Background = 0,
    BackObject = 1,
    StageObject = 2,
    Enemy = 3,
    Player = 4,
    FrontObject = 5,
    UI = 6
};

struct DrawItem {
    Object* obj;
    DrawLayer layer;
};

class StageBase {
protected:
    bool m_isGoalReached = false;

    DirectX::XMFLOAT3 m_currentCheckpoint = { 10000, 0, 0 };
    bool m_hasCheckpoint = false;

public:
    virtual ~StageBase() {}

    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void UnInit() = 0;

    virtual bool IsPlayerDead() = 0;
    virtual bool IsGoalReached() const { return m_isGoalReached; }

    virtual DirectX::XMFLOAT3 GetRespawnPosition() const { return m_currentCheckpoint; }
    virtual void SetRespawnPos(DirectX::XMFLOAT3 respawnpos) { m_currentCheckpoint = respawnpos; }

    friend class StageManager;
};

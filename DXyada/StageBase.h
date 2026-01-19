#pragma once
#include <vector>
#include "Platform.h"
#include "Enemy.h"
#include "Texture.h"
#include "Player.h"
#include "Camera2D.h"
#include "CollisionManager.h"
#include "Ribbon.h"
#include "Pin.h"
#include "BlockPin.h"
#include "RemoteWindPin.h"
#include "Tutorial.h"

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
public:
    virtual ~StageBase() {}

    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void UnInit() = 0;

    virtual bool IsPlayerDead() = 0;
};

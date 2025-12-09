#pragma once
#include "SceneBase.h"
#include "Object.h"
#include "player.h"
#include"Platform.h"
#include "CollisionManager.h"

class SceneMain : public SceneBase {
private:
    CollisionManager m_collision;

    Player m_player;
    Object m_background;

    Platform m_block;
    Platform m_block1;
    Object d_block;

public:
    void Init() override;
    void Update(SceneManager& mgr) override;
    void Draw() override;
    void UnInit() override;
};

#pragma once
#include "StageBase.h"

class DebugStage : public StageBase {
private:

    Object m_background;
    Player m_player;
    Camera m_camera;
    CollisionManager* m_collision;

    std::vector<Platform> m_platforms;
    std::vector<Enemy*> m_enemies;

    std::vector<DrawItem> m_drawList;

    bool m_isPlayerDead = false;

public:
    void Init() override;
    void Update() override;
    void Draw() override;
    void UnInit() override;

    bool IsPlayerDead() override { return m_isPlayerDead; }
};

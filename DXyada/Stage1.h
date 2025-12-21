#pragma once
#include "StageBase.h"


class Stage1 : public StageBase {
private:
    Object m_background;
    Player m_player;
    Camera m_camera;
    CollisionManager* m_collision;
    Object m_mae;
    Object m_usiro;

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

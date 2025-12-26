#pragma once
#include "StageBase.h"
#include "Pin.h"


class Stage1 : public StageBase {
private:
    Object m_background;
    Player m_player;
    Camera m_camera;
    CollisionManager* m_collision;
    Object m_mae;
    Object m_usiro;

    Object m_HP_UI1;
    Object m_HP_UI2;
    Object m_HP_UI3;

    std::vector<Platform> m_platforms;
    std::vector<Enemy*> m_enemies;
    std::vector<Pin*> m_pins;

    std::vector<DrawItem> m_drawList;

    bool m_isPlayerDead = false;

public:
    void Init() override;
    void Update() override;
    void Draw() override;
    void UnInit() override;

    bool IsPlayerDead() override { return m_isPlayerDead; }

    int maxHP = m_player.GetMaxHP();
    int currentHP = m_player.GetHP();
};

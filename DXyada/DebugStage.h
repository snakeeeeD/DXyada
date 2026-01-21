#pragma once
#include "StageBase.h"

class DebugStage : public StageBase {
private:

    Object m_background;
    Player m_player;
    Camera m_camera;
    CollisionManager* m_collision;

    Object m_HP_UI1;
    Object m_HP_UI2;
    Object m_HP_UI3;

    std::vector<Platform*> m_platforms;
    std::vector<Enemy*> m_enemies;
    std::vector<Pin*> m_pins;
    DirectX::XMFLOAT2 m_BlockPinPos;

    std::vector<DrawItem> m_drawList;

    bool m_isPlayerDead = false;

public:
    void Init() override;
    void Update() override;
    void Draw() override;
    void UnInit() override;

    void AddPlatform(Platform& plat, bool isMoving);

    bool IsPlayerDead() override { return m_isPlayerDead; }

    int maxHP = m_player.GetMaxHP();
    int currentHP = m_player.GetHP();

    RemoteWindPin* m_hook = nullptr;
    BlockPin* m_targetPin = nullptr;
};

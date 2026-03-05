#pragma once
#include "StageBase.h"

#include "CollisionManager.h"
#include "Camera2D.h"
#include "Object.h"
#include "Player.h"
#include "Platform.h"
#include "Enemy.h"
#include "Pin.h"
#include "BlockPin.h"
#include "Goal.h"
#include "Tutorial.h"
#include "DecoMarker.h"
#include "Rippa.h"


class BossStage : public StageBase
{
public:
    BossStage() = default;
    ~BossStage() override = default;

    void Init() override;
    void Update() override;
    void Draw() override;
    void UnInit() override;

    bool IsPlayerDead() override { return m_isPlayerDead; }
    bool IsGoalReached() const override { return m_isGoalReached; }

    void Respawn();
    DirectX::XMFLOAT3 GetRespawnPosition() const override { return m_currentCheckpoint; }
    void SetRespawnPos(DirectX::XMFLOAT3 respos) override;

private:
    // ---- stage objects ----
    CollisionManager* m_collision = nullptr;
    Camera            m_camera;
    DirectX::XMFLOAT3 m_cameraNowPos{};

    Object m_background;
    Player m_player;

    Object m_HP_UI1;
    Object m_HP_UI2;
    Object m_HP_UI3;

    int maxHP = 0;
    int currentHP = 0;

    std::vector<Platform>  m_platforms;     
    std::vector<Platform>  m_oneWayPlatforms; 
    std::vector<Enemy*>    m_enemies;
    std::vector<Pin*>      m_pins;
    std::vector<Goal*>     m_goal;

    std::vector<Object>    m_fuwafuwas;

    std::vector<DrawItem>  m_drawList;
    void BuildDrawList();

    bool  m_isPlayerDead = false;
    float m_fallDeadLineY = -1200.0f;

    void AddPlatformStatic(const char* tex, float x, float y, float w, float h);
    void AddPlatformOneWay(const char* tex, float x, float y, float w, float h);

    void AddDecorPin(float x, float y, bool canDecorate);
    BlockPin* AddPullPin(float x, float y, bool canRollPin);

    void RegisterEnemyMarker(Enemy* enemy, const char* markerTex, bool isMove);
    void RegisterPinMarker(Pin* pin, const char* markerTex, bool isMove);
    void FinalizeCollisionRegistrations();
    struct DecoLinkPin
    {
        Pin* pin = nullptr;
        DecoMarker marker;
    };
    struct DecoLinkEnemy
    {
        Enemy* enemy = nullptr;
        DecoMarker marker;
    };
    std::vector<DecoLinkPin>   m_pinMarkers;
    std::vector<DecoLinkEnemy> m_enemyMarkers;
};

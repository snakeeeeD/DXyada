#pragma once
#include "StageBase.h"

class Stage1 : public StageBase
{
public:
    Stage1() = default;
    ~Stage1() override = default;

    void Init() override;
    void Update() override;
    void Draw() override;
    void UnInit() override;

    bool IsPlayerDead() override { return m_isPlayerDead; }
    bool IsGoalReached() const override { return m_isGoalReached; }

    void Respawn();
    DirectX::XMFLOAT3 GetRespawnPosition() const { return m_currentCheckpoint; }
    void SetResoawnPos(DirectX::XMFLOAT3 respos);

    void RegisterEnemyMarker(Enemy* enemy, const char* markerTex, bool isMove);

    int  StageState = 0;
    bool on = false;

private:

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
    std::vector<Enemy*>    m_enemies;
    std::vector<Pin*>      m_pins;
    std::vector<Tutorial*> m_tutorials;
    std::vector<Goal*> m_goal;   

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


    std::vector<DrawItem> m_drawList;
    void BuildDrawList();


    bool m_isGoalReached = false;
    bool  m_isPlayerDead = false;
    float m_fallDeadLineY = -800.0f;

    DirectX::XMFLOAT3 m_currentCheckpoint{};
    bool              m_hasCheckpoint = false;


    void AddPlatform(const char* tex, float x, float y, float w, float h);
    void AddDecorPin(float x, float y, bool canDecorate);
    BlockPin* AddPullPin(float x, float y, bool canRollPin);

private:
  
    BlockPin* m_targetPin2 = nullptr; 
};

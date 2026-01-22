#pragma once
#include "StageBase.h"

class TutorialStage2 : public StageBase
{
private:
    Object m_background;
    Player m_player;
    Camera m_camera;
    DirectX::XMFLOAT3 m_cameraNowPos;
    CollisionManager* m_collision = nullptr;

    Object m_HP_UI1;
    Object m_HP_UI2;
    Object m_HP_UI3;

    std::vector<Platform> m_platforms;
    std::vector<Enemy*>   m_enemies;
    std::vector<Pin*>     m_pins;

    std::vector<DrawItem> m_drawList;

    bool m_isPlayerDead = false;

    int maxHP = 0;
    int currentHP = 0;

    float m_fallDeadLineY = -800.0f;

    bool m_cameraMoveDown = false;
    float m_cameraTargetY = 0.0f;
    float m_cameraYOffset = 0.0f;       // 現在オフセット
    float m_cameraTargetOffset = 0.0f;  // 目標オフセット

    DirectX::XMFLOAT3 m_currentCheckpoint;
    bool m_hasCheckpoint;

private:
    void AddPlatform(const char* tex, float x, float y, float w, float h);
    void AddDecorPin(float x, float y, bool canDecorate);
    BlockPin* AddPullPin(float x, float y, bool canRollPin);
    void BuildDrawList();

public:
    BlockPin* m_targetPin2 = new BlockPin;
    void Init() override;
    void Update() override;
    void Draw() override;
    void UnInit() override;

    bool IsPlayerDead() override { return m_isPlayerDead; }

    void Respawn();  //リスポーン処理

    DirectX::XMFLOAT3 GetRespawnPosition() const { return m_currentCheckpoint; }
    void SetResoawnPos(DirectX::XMFLOAT3 respos);
};

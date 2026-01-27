#pragma once
#include "SceneBase.h"
#include "SceneID.h"
#include "Object.h"
#include "Player.h"
#include <vector>

class SceneManager;

class SceneLoad : public SceneBase
{
public:
    enum class ObstacleType
    {
        WingRippa,
        Rippa,
        NeedleFloor
    };

    enum MiniAnim { MINI_WALK, MINI_JUMP, MINI_DAMAGE };
    MiniAnim m_miniAnim = MINI_WALK;
    bool m_prevJumping = false;

    SceneLoad(SceneID next = SCENE_GAME) : m_next(next) {}
    virtual ~SceneLoad() {}

    void Init() override;
    void Update(SceneManager& mgr) override;
    void Draw() override;
    void UnInit() override;

private:
    void BuildQueue();

    void MiniInit();
    void MiniUpdate(float dt);
    void MiniDraw();
    void MiniReset();

    bool MiniCheckHitAABB(const Object& a, const Object& b);

    ObstacleType PickRandomType();
    void SpawnObstacle();


private:
    SceneID m_next = SCENE_GAME;

    Object m_bg;

    Object m_ribbonGauge;

    Player m_previewPlayer;
    bool m_previewReady = false;
    bool m_wasDone = false;
    bool m_done = false;

    int m_loadPerFrame = 1;
    int m_frame = 0;
    int m_loadInterval = 3;

    enum Mode
    {
        MODE_LOADING = 0,
        MODE_READY,
        MODE_MINI
    };
    Mode m_mode = MODE_LOADING;

    Object m_ground;
    Object m_playerBox;

    float m_groundY = -250.0f;
    float m_jumpY = -120.0f;
    float m_playerY = -250.0f;

    bool m_jumping = false;
    float m_jumpDelay = 0.00f;
    float m_jumpT = 0.0f;
    float m_jumpDuration = 0.50f;
    float m_jumpDelayT = 0.0f;

    struct Obstacle
    {
        Object obj;
        bool alive;
        ObstacleType type;
        float requiredJumpY = 0.0f;
    };

    std::vector<Obstacle> m_obs;

    float m_speed = 900.0f;
    float m_spawnTimer = 0.0f;
    float m_spawnInterval = 0.9f;
    bool m_gameOver = false;

    float m_score = 0.0f;

    bool m_jumpHeld = false;
    float m_jumpHoldT = 0.0f;

    float m_jumpY_Short = 0.0f;
    float m_jumpY_Long = 0.0f;

    float m_jumpHoldMax = 0.18f;
    float m_jumpYBoost = 220.0f;

    int m_spawnIndex = 0;

    float m_spawnIntervalBase;
    float m_spawnIntervalRand;

    ObstacleType m_lastType;
    int m_sameCount = 0;

    float m_miniOffsetY = 420.0f;
    float m_miniScale = 0.70f;

    bool m_miniDamageLock = false;

    float m_groundUV = 0.0f;
    float  m_gaugeT = 0.0f;   
    float  m_gauge01 = 0.0f;  
    bool   m_gaugeStarted = false;

    float m_gaugeFinalW = 600.0f;
    float m_gaugeFinalH = 80.0f;
    float m_gaugeLeftX = 0.0f; 
    float m_gaugeY = 350.0f;

    bool m_gaugeJustMax = false;
};

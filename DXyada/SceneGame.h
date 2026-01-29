#pragma once
#include "SceneBase.h"
#include "StageBase.h"
#include "StageManager.h"

enum class GameState
{
    Playing,
    Pouse,
    Result,
    GameOver
};

struct ResultCam
{
    bool active = false;
    float t = 0.0f;          // 経過(秒)
    float duration = 8.0f;   // 演出時間(秒)

    DirectX::XMFLOAT3 start = { 0,0,0 };
    DirectX::XMFLOAT3 goal = { 0,0,0 };
};

class SceneGame : public SceneBase {
private:
    int m_stageNumber = 1;
    StageManager m_stageManager;
    GameState m_state = GameState::Playing;

    ResultCam m_resultCam;

    Object Pouse_BackGround;
    Object GameOver_BackGround;
    Object m_cursor;
    Object m_Buttonretry;
    Object m_Buttoncontinue;
    Object m_ButtonStageselect;
    Object m_ResultUI;
    Object m_Star1;
    Object m_Star2;
    Object m_Star3;


    int SizeMin = 730;
    int SizeMax = 790;
    int m_size = 730;

    int Star = 0;
    int m_prevStar = -1;

    int deltaTime = 0;

    bool Big = false;
    bool Small = false;

    int m_cursorNum = 0;

    //左スティックトリガー用
    bool m_prevLeftStick = false;

    int m_stickNow = 0;

    bool Result_Next = false;

    bool m_bgmGamePlaying = false;
    bool m_bgmClearPlaying = false;
    bool m_bgmGameOverPlaying = false;

public:
    SceneGame(int stageNum = 1);
    virtual void Init() override;
    virtual void Update(SceneManager& mgr) override;
    void UpdatePlaying(SceneManager& mgr);
    void UpdatePouse(SceneManager& mgr);
    void UpdateResult(SceneManager& mgr);
    void UpdateGameOver(SceneManager& mgr);
    virtual void Draw() override;
    virtual void UnInit() override;

    DirectX::XMFLOAT3 GetResultStartCamPos(int stageNum) const;
    DirectX::XMFLOAT3 GetResultGoalCamPos(int stageNum) const;
    void BeginResult(SceneManager& mgr);
};
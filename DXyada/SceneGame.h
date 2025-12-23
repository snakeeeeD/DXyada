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

class SceneGame : public SceneBase {
private:
    int m_stageNumber = 1;
    StageManager m_stageManager;
    GameState m_state = GameState::Playing;

    Object Pouse_BackGround;
    Object GameOver_BackGround;
    Object m_cursor;
    Object m_Buttonretry;
    Object m_Buttoncontinue;
    Object m_ButtonStageselect;

    int SizeMin = 190;
    int SizeMax = 210;
    int m_size = 190;

    int deltaTime = 0;

    bool Big = false;
    bool Small = false;

    int m_cursorNum = 0;

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
};
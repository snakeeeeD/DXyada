#pragma once
#include "SceneBase.h"
#include "StageBase.h"
#include "StageManager.h"

class SceneGame : public SceneBase {
private:
    int m_stageNumber = 1;
    StageManager m_stageManager;

public:
    SceneGame(int stageNum = 1);
    virtual void Init() override;
    virtual void Update(SceneManager& mgr) override;
    virtual void Draw() override;
    virtual void UnInit() override;
};
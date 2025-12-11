// SceneTitle.h
#pragma once
#include "SceneBase.h"
#include "SceneManager.h"

class SceneTitle : public SceneBase {
private:
    int m_stage = 1;
    Object m_DebugStage;
public:
    void Init() override;
    void Update(SceneManager& mgr) override;
    void Draw() override;
    void UnInit() override;
};

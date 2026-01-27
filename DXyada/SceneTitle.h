// SceneTitle.h
#pragma once
#include "SceneBase.h"
#include "SceneManager.h"

class SceneTitle : public SceneBase {
private:
    int m_stage = 1;
    float m_loadDelay = 0.0f;

    int Loadtime=0;
public:
    void Init() override;
    void Update(SceneManager& mgr) override;
    void Draw() override;
    void UnInit() override;
};

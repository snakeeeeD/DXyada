#pragma once
#include "SceneBase.h"
#include "Object.h"

class SceneLoading : public SceneBase
{
private:
    Object m_bg;
    Object m_bar;

    // 追加：回転スピナー
    Object m_spinner;
    float  m_spinAngle = 0.0f;   // 角度
    float  m_spinSpeed = 180.0f; // 度/秒（好みで 90, 180, 360 など）

    float m_lastP = 0.0f;

public:
    void Init() override;
    void Update(SceneManager& mgr) override;
    void Draw() override;
    void UnInit() override;

};

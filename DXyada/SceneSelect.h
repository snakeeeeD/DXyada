#pragma once
#include "SceneBase.h"
#include "SceneManager.h"
#include "Camera2D.h"
#include "Object.h"

class SceneSelect : public SceneBase {
private:
    Object m_select;
    Object m_stage1;
    Object m_stage2;
    Object m_stage3;
    Object m_cursor;
    Camera m_camera;

    int m_stage = 1;

    int SizeMin = 100;
    int SizeMax = 120;
    int m_size = 100;

    int deltaTime = 0;

    bool Big = false;
    bool Small = false;

    int MAX_STAGE = 3;

public:
    void Init() override;
    void Update(SceneManager& mgr) override;
    void Draw() override;
    void UnInit() override;

};
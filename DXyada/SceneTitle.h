// SceneTitle.h
#pragma once
#include "SceneBase.h"
#include "SceneManager.h"

class SceneTitle : public SceneBase {
public:
    void Init() override;
    void Update(SceneManager& mgr) override;
    void Draw() override;
    void UnInit() override;
};

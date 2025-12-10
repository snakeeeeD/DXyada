#pragma once
#include "SceneBase.h"
#include"SceneManager.h"
#include"objbase.h"

class SceneManager;

class SceneGameOver : public SceneBase {
private:
    Object m_select;
public:
    void Init() override;
    void Update(SceneManager&) override;
    void Draw() override;
    void UnInit() override;
};

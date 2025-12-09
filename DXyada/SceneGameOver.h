#pragma once
#include "SceneBase.h"

class SceneManager;

class SceneGameOver : public SceneBase {
public:
    void Init() override {}
    void Update(SceneManager&) override {}
    void Draw() override {}
    void UnInit() override {}
};

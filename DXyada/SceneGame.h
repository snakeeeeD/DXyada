#pragma once
#include "SceneBase.h"
#include "StageManager.h"
#include "Player.h"
#include "CollisionManager.h"
#include "Renderer.h"
#include "Camera2D.h"

class SceneManager;

class SceneGame : public SceneBase {
private:
    Player m_player;
    StageManager m_stageManager;
    Camera m_camera;
    CollisionManager* m_collision;

public:
    SceneGame() = default;
    ~SceneGame() override { UnInit(); }

    void Init() override;
    void Update(SceneManager& mgr) override;
    void Draw() override;
    void UnInit() override;
};

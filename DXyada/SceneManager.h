#pragma once
#include "SceneBase.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include "SceneGameOver.h"
#include "SceneID.h"

class SceneManager {
private:
    SceneBase* m_currentScene = nullptr;
    SceneID m_currentID;

public:
    SceneManager() = default;
    ~SceneManager() { UnInit(); }

    void Init(SceneID id);
    void Update();
    void Draw();
    void UnInit();

    void ChangeScene(SceneID id);
};

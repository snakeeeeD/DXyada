#pragma once
#include "SceneGame.h"
#include "SceneSelect.h"
#include "SceneTitle.h"
#include "SceneGameOver.h"
#include "SceneID.h"

class SceneManager {
private:
    SceneBase* m_currentScene = nullptr;
    SceneID m_currentID;

    int m_selectedStage = 1;
public:
    SceneManager() = default;
    ~SceneManager() { UnInit(); }

    void SetSelectedStage(int stage) { m_selectedStage = stage; }
    int  GetSelectedStage() const { return m_selectedStage; }

    void Init(SceneID id);
    void Update();
    void Draw();
    void UnInit();

    void ChangeScene(SceneID id);
};
#pragma once
#include <memory>
#include <vector>
#include "StageBase.h"
#include "Stage1.h"

class StageManager {
private:
    std::unique_ptr<StageBase> m_currentStage;

public:
    StageManager() {}
    ~StageManager() { Uninit(); }

    void LoadStage(int stageNumber);
    void Update();
    void Draw();
    void Uninit();

    std::vector<Platform>& GetPlatforms();
    std::vector<Enemy>& GetEnemy();

   // Stage* GetCurrentStage() { return m_currentStage; }
};

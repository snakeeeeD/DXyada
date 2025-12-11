#pragma once
#include <memory>
#include <vector>
#include "StageBase.h"
#include "Stage1.h"
#include "Stage2.h"

class StageManager {
private:
    std::unique_ptr<StageBase> m_currentStage;

public:
    void LoadStage(int stageNumber) {
        Uninit();

        switch (stageNumber) {
        case 1:
            m_currentStage = std::make_unique<Stage1>();
            break;
        case 2:
            m_currentStage = std::make_unique<Stage2>();
            break;
        default:
            m_currentStage = nullptr;
            break;
        }

        if (m_currentStage)
            m_currentStage->Init();
    }

    void Update() {
        if (m_currentStage)
            m_currentStage->Update();
    }

    void Draw() {
        if (m_currentStage)
            m_currentStage->Draw();
    }

    void Uninit() {
        if (m_currentStage) {
            m_currentStage->UnInit();
            m_currentStage.reset();
        }
    }

    StageBase* GetStage() {
        return m_currentStage.get();
    }
};

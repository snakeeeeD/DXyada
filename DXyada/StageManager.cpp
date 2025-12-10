#include "StageManager.h"

void StageManager::LoadStage(int stageNumber) {
    Uninit();

    switch (stageNumber) {
    case 1:
        m_currentStage = std::unique_ptr<StageBase>(static_cast<StageBase*>(new Stage1()));
        break;
    case 2:
        m_currentStage = std::unique_ptr<StageBase>(static_cast<StageBase*>(new Stage2()));
        break;
    default:
        m_currentStage = nullptr;
        break;
    }

    if (m_currentStage) {
        m_currentStage->Init();
    }
}

void StageManager::Update() {
    if (m_currentStage) {
        m_currentStage->Update();
    }
}

void StageManager::Draw() {
    if (m_currentStage) {
        m_currentStage->Draw();
    }
}

void StageManager::Uninit() {
    if (m_currentStage) {
        m_currentStage->UnInit();
        m_currentStage.reset();
    }
}

std::vector<Platform>& StageManager::GetPlatforms() {
    Stage1* stage1 = dynamic_cast<Stage1*>(m_currentStage.get());
    Stage2* stage2 = dynamic_cast<Stage2*>(m_currentStage.get());
    if (stage1) {
        return const_cast<std::vector<Platform>&>(stage1->GetPlatforms());
    } 
    if (stage2) {
        return const_cast<std::vector<Platform>&>(stage2->GetPlatforms());
    }
    static std::vector<Platform> empty;
    return empty;
}


std::vector<Enemy>& StageManager::GetEnemy() {
    Stage1* stage1 = dynamic_cast<Stage1*>(m_currentStage.get());
    Stage2* stage2 = dynamic_cast<Stage2*>(m_currentStage.get());
    if (stage1) {
        return const_cast<std::vector<Enemy>&>(stage1->GetEnemy());
    }
    if (stage2) {
        return const_cast<std::vector<Enemy>&>(stage2->GetEnemy());
    }
    static std::vector<Enemy> empty;
    return empty;
}

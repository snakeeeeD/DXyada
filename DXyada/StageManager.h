#pragma once
#include <memory>
#include <vector>
#include "StageBase.h"
#include "Stage1.h"
#include "Stage2.h"
#include "TutorialStage.h"
#include "DebugStage.h"

class StageManager {
private:
    std::unique_ptr<StageBase> m_currentStage;
    int m_currentStageNumber = -1;  //現在のステージ番号
    DirectX::XMFLOAT3 m_savedCheckpoint = { 0,0,0 }; //保存されたチェックポイント
    bool m_hasCheckpoint = false;

public:

    void LoadStage(int stageNumber, bool fromRetry = false) {
 
        if (m_currentStage && m_currentStage->m_hasCheckpoint) {
            m_savedCheckpoint = m_currentStage->GetRespawnPosition();
            m_hasCheckpoint = true;
        }

        if (!fromRetry || m_currentStageNumber != stageNumber) {
            m_hasCheckpoint = false;
            m_savedCheckpoint = { 0, 0, 0 };
        }

        m_currentStageNumber = stageNumber;

        DirectX::XMFLOAT3 tempCheckpoint = m_savedCheckpoint;
        bool tempHasCheckpoint = m_hasCheckpoint;

        if (m_currentStage) {
            m_currentStage->UnInit();
            m_currentStage.reset();
        }

        switch (stageNumber) {
        case 1:  m_currentStage = std::make_unique<Stage1>(); break;
        case 2:  m_currentStage = std::make_unique<TutorialStage2>(); break;
        case 3:  m_currentStage = std::make_unique<TutorialStage>(); break;
        case 99: m_currentStage = std::make_unique<DebugStage>(); break;
        default: m_currentStage = nullptr; break;
        }

        if (m_currentStage) {
            if (tempHasCheckpoint) {
                m_currentStage->SetRespawnPos(tempCheckpoint);
                m_currentStage->m_hasCheckpoint = true;
            }

            m_currentStage->Init();
        }
    }

    void Update() {
        if (m_currentStage)
            m_currentStage->Update();
    }

    void Draw() {
        if (m_currentStage)
            m_currentStage->Draw();
    }

    //void Uninit() {
    //    if (m_currentStage) {
    //        // ステージ側でチェックポイントが一度でも有効になっていれば保存する
    //        if (m_currentStage->m_hasCheckpoint) {
    //            m_savedCheckpoint = m_currentStage->GetRespawnPosition();
    //            m_hasCheckpoint = true; // マネージャー側のフラグを立てる
    //        }

    //        m_currentStage->UnInit();
    //        m_currentStage.reset();
    //    }
    //}

    void Uninit() {
        if (m_currentStage) {
            // ★チェックポイントを保存（破棄前に）
            if (m_currentStage->m_hasCheckpoint) {
                m_savedCheckpoint = m_currentStage->GetRespawnPosition();
                m_hasCheckpoint = true;

                char buf[256];
                sprintf_s(buf, "Uninit: Saved checkpoint (%.1f, %.1f, %.1f)\n",
                    m_savedCheckpoint.x, m_savedCheckpoint.y, m_savedCheckpoint.z);
                OutputDebugStringA(buf);
            }

            m_currentStage->UnInit();
            m_currentStage.reset();
        }
    }

    StageBase* GetStage() {
        return m_currentStage.get();
    }

    //ステージセレクトに戻る時にチェックポイントをクリア
    void ResetCheckpoint() {
        m_hasCheckpoint = false;
        m_savedCheckpoint = { 0, 0, 0 };
        m_currentStageNumber = -1;
    }
};

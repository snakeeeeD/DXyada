// RemoteWindPin.h
#pragma once
#include "Pin.h"
#include "Object.h"
#include <vector>
#include <DirectXMath.h>

class RemoteWindPin : public Pin
{
public:
    // Pin と同じ形で初期化できるようにする
    void Init(const char* texture, float x, float y, float w, float h);

    // 巻き取り対象（BlockPin* でも Pin* に暗黙変換される）
    void SetTarget(Pin* target);

    // ガイド操作
    void ClearGuides();
    void AddGuide(const DirectX::XMFLOAT3& p);

    // 巻き取り（プレイヤー側から呼ばれる想定）
    void OnWindUp(const DirectX::XMFLOAT3& playerPos, float dt, float pullSpeed);

    // ガイド描画（Stage/Scene 側から呼ぶ）
    void DrawGuides();

    // 任意：デバッグ表示ON/OFF
    void SetDrawGuides(bool enable) { m_drawGuides = enable; RebuildGuideObjects(); }

    // 任意：見た目調整
    void SetGuideTexture(const char* tex) { m_guideTex = tex; RebuildGuideObjects(); }
    void SetGuideSize(float size) { m_guideSize = size; RebuildGuideObjects(); }

private:
    // ターゲット
    Pin* m_target = nullptr;

    // ガイド点列
    std::vector<DirectX::XMFLOAT3> m_guides;
    int  m_guideIndex = 0;

    // 終了フラグ（最後まで行ったら true）
    bool m_finished = false;

    // 到達判定（ピクセル）
    float m_reachEps = 2.0f;

    // 最小ステップ（0なら無効）
    float m_minStep = 0.0f;

private:
    void MoveTargetAlongGuides(float step);
    void Finish();

private:
    // ==== ガイド描画（AddTextureだけ） ====
    std::vector<Object> m_guideObjects;
    bool  m_drawGuides = true;
    float m_guideSize = 12.0f;
    const char* m_guideTex = "asset/ui/cursor.png";

    float m_guideReachedScale = 2.0f; // 到達したら2倍

    void RebuildGuideObjects();
};

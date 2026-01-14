#pragma once
#include "BlockPin.h"
#include <vector>

class MakitoriBlock : public BlockPin
{
private:
    // 紐で引っ張られる側（ブロック本体）
    Object m_block;

    // ガイド点（自由に追加）
    std::vector<DirectX::XMFLOAT3> m_guides;
    int m_guideIndex = 0;

    bool m_finished = false;
    float m_reachEps = 10.0f;   // ガイド到達判定の許容誤差（px）

private:
    void MoveBlockAlongGuides(float pullDistance);

public:
    MakitoriBlock() {}

    // ピン＋ブロックの初期化
    // pinTex: ピンの画像（既存Pin.pngなど）
    // blockTex: 引っ張られるブロック画像（block.pngなど）
    void Init(
        const char* pinTex, float pinX, float pinY, float pinW, float pinH,
        const char* blockTex, float blockX, float blockY, float blockW, float blockH
    );

    // ブロック本体へのアクセス（Stage側でCollision登録・DrawList登録に使う）
    Object* GetBlockObject() { return &m_block; }

    // ガイド設定
    void ClearGuides();
    void AddGuide(const DirectX::XMFLOAT3& p);
    void SetReachEps(float eps) { m_reachEps = eps; }

    bool IsFinished() const { return m_finished; }

    // 重要：ピンは動かない前提の巻き取り処理
    void OnWindUp(const DirectX::XMFLOAT3& playerPos, float dt, float pullSpeed) override;

    // 後始末
    void UnInit() override;
};

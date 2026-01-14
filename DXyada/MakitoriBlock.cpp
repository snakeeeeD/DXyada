#include "MakitoriBlock.h"
#include <cmath>
#include <algorithm>

static float Len2D(float x, float y)
{
    return std::sqrt(x * x + y * y);
}

void MakitoriBlock::Init(
    const char* pinTex, float pinX, float pinY, float pinW, float pinH,
    const char* blockTex, float blockX, float blockY, float blockW, float blockH
)
{
    // 親（BlockPin = Pin）初期化：ピン本体
    BlockPin::Init(pinTex, pinX, pinY, pinW, pinH);

    // ブロック本体
    m_block.Init();
    m_block.AddTexture(blockTex);
    m_block.SetPos(blockX, blockY, 0);
    m_block.SetSize(blockW, blockH, 0);

    // 初期状態
    m_guides.clear();
    m_guideIndex = 0;
    m_finished = false;

    // 巻き取り可能（必要に応じてStage側で変えてOK）
    SetcanRollPin(true);
    SetcanDecorate(false);
}

void MakitoriBlock::ClearGuides()
{
    m_guides.clear();
    m_guideIndex = 0;
    m_finished = false;
    SetcanRollPin(true);
}

void MakitoriBlock::AddGuide(const DirectX::XMFLOAT3& p)
{
    m_guides.push_back(p);

    // 途中追加で終了状態になっていた場合は復帰させる
    if (m_finished)
    {
        m_finished = false;
        if (m_guideIndex >= (int)m_guides.size()) m_guideIndex = (int)m_guides.size() - 1;
        SetcanRollPin(true);
    }
}

void MakitoriBlock::MoveBlockAlongGuides(float pullDistance)
{
    if (m_finished) return;
    if (m_guides.empty()) { m_finished = true; SetcanRollPin(false); return; }
    if (m_guideIndex >= (int)m_guides.size()) { m_finished = true; SetcanRollPin(false); return; }

    DirectX::XMFLOAT3 pos = m_block.GetPos();

    // 現在ガイド
    DirectX::XMFLOAT3 target = m_guides[m_guideIndex];

    float dx = target.x - pos.x;
    float dy = target.y - pos.y;
    float dist = Len2D(dx, dy);

    // 近ければ次へ
    if (dist <= m_reachEps)
    {
        m_guideIndex++;
        if (m_guideIndex >= (int)m_guides.size())
        {
            m_finished = true;
            SetcanRollPin(false);
            return;
        }

        target = m_guides[m_guideIndex];
        dx = target.x - pos.x;
        dy = target.y - pos.y;
        dist = Len2D(dx, dy);
        if (dist <= 0.0001f) return;
    }

    // 今回の巻き取り距離だけ直進（ガイドを超えない）
    float step = (std::min)(pullDistance, dist);

    float nx = dx / dist;
    float ny = dy / dist;

    pos.x += nx * step;
    pos.y += ny * step;

    m_block.SetPos(pos.x, pos.y, pos.z);

    // 到達判定（このフレームで繰り越しはしない：安定優先）
    if (step >= dist - m_reachEps)
    {
        m_guideIndex++;
        if (m_guideIndex >= (int)m_guides.size())
        {
            m_finished = true;
            SetcanRollPin(false); // ガイドが尽きたら「これ以上巻けない」
        }
    }
}

void MakitoriBlock::OnWindUp(const DirectX::XMFLOAT3& /*playerPos*/, float dt, float pullSpeed)
{
    // ガイドが尽きているなら巻けない
    if (m_finished || m_guides.empty() || m_guideIndex >= (int)m_guides.size())
    {
        m_finished = true;
        SetcanRollPin(false);
        return;
    }

    // ピンは動かない前提なので、巻き取り量を距離に変換
    float pullDistance = pullSpeed * dt;
    if (pullDistance < 0.0f) pullDistance = 0.0f;

    MoveBlockAlongGuides(pullDistance);
}

void MakitoriBlock::UnInit()
{
    // ブロック本体
    m_block.UnInit();

    // ピン本体
    BlockPin::UnInit();
}

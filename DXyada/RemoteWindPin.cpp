// RemoteWindPin.cpp
#include "RemoteWindPin.h"
#include <cmath>
#include <algorithm>

// あなたのプロジェクトで別ファイルに extern があるなら、重複しないようにここは削除してください。
// DebugStage.cpp と同じ Draw 呼び出し形式に合わせています。
extern ID3D11DeviceContext* g_pDeviceContext;
extern ID3D11InputLayout* g_pInputLayout;
extern ID3D11VertexShader* g_pVertexShader;
extern ID3D11PixelShader* g_pPixelShader;
extern ID3D11Buffer* g_pConstantBuffer;

static float Len2D(float x, float y)
{
    return std::sqrt(x * x + y * y);
}

void RemoteWindPin::Init(const char* texture, float x, float y, float w, float h)
{
    // Pinとして初期化（ここが重要：Init の形を統一）
    Pin::Init(texture, x, y, w, h);

    // RemoteWindPin は「巻き取り用」なので基本こう
    SetcanRollPin(true);
    SetcanDecorate(false);

    m_target = nullptr;
    m_guides.clear();
    m_guideIndex = 0;
    m_finished = false;

    RebuildGuideObjects();
}

void RemoteWindPin::SetTarget(Pin* target)
{
    m_target = target;

    // ターゲット設定時点で必ず巻ける状態に戻す（巻けなくなった問題の保険）
    m_finished = false;
    SetcanRollPin(true);
}

void RemoteWindPin::ClearGuides()
{
    m_guides.clear();
    m_guideIndex = 0;
    m_finished = false;
    SetcanRollPin(true);

    RebuildGuideObjects();
}

void RemoteWindPin::AddGuide(const DirectX::XMFLOAT3& p)
{
    m_guides.push_back(p);

    // 既に終わっていた場合、ガイド追加で復帰させる
    if (m_finished)
    {
        m_finished = false;
        SetcanRollPin(true);

        if (m_guideIndex >= (int)m_guides.size())
            m_guideIndex = (int)m_guides.size() - 1;
    }

    RebuildGuideObjects();
}

void RemoteWindPin::Finish()
{
    // ここで canRollPin を落とすと、誤Finishで永久に巻けなくなるので状態だけにする
    SetcanRollPin(false);
    m_finished = true;
}

void RemoteWindPin::MoveTargetAlongGuides(float step)
{
    if (m_finished) return;

    // 対象 or ガイドが無いなら終了
    if (!m_target) { Finish(); return; }
    if (m_guides.empty()) { Finish(); return; }
    if (m_guideIndex >= (int)m_guides.size()) { Finish(); return; }

    Object* obj = m_target->GetObject();
    if (!obj) { Finish(); return; }

    DirectX::XMFLOAT3 pos = obj->GetPos();

    // 現在の目標ガイド
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
            Finish();
            return;
        }

        target = m_guides[m_guideIndex];
        dx = target.x - pos.x;
        dy = target.y - pos.y;
        dist = Len2D(dx, dy);

        if (dist <= 0.0001f) return;
    }

    // 進める量（ガイドを超えない）
    float move = step;
    if (move < 0.0f) move = 0.0f;

    if (m_minStep > 0.0f && move < m_minStep) move = m_minStep;

    // Windows min マクロ対策
    move = (std::min)(move, dist);

    float nx = dx / dist;
    float ny = dy / dist;

    pos.x += nx * move;
    pos.y += ny * move;

    obj->SetPos(pos.x, pos.y, pos.z);

    // 到達したら次へ（繰り越しはしない：安定優先）
    if (move >= dist - m_reachEps)
    {
        m_guideIndex++;
        if (m_guideIndex >= (int)m_guides.size())
        {
            Finish();
        }
    }
}

void RemoteWindPin::OnWindUp(const DirectX::XMFLOAT3& /*playerPos*/, float dt, float pullSpeed)
{
    // 既に終了なら巻けない
    if (m_finished)
    {
        SetcanRollPin(false);
        return;
    }

    // 対象 or ガイドがないなら終了
    if (!m_target || m_guides.empty())
    {
        Finish();
        SetcanRollPin(false);
        return;
    }

    // 巻き取り量
    float step = pullSpeed * dt;
    if (step < 0.0f) step = 0.0f;

    MoveTargetAlongGuides(step);

    // 最後まで行ったら以降巻けない
    if (m_finished)
    {
        SetcanRollPin(false);
    }
}

// ------------------------------------------------------------
// ガイド描画（AddTextureだけ）
// ------------------------------------------------------------
void RemoteWindPin::RebuildGuideObjects()
{
    m_guideObjects.clear();

    if (!m_drawGuides) return;
    if (m_guides.empty()) return;
    if (!m_guideTex) return;

    m_guideObjects.reserve(m_guides.size());

    for (size_t i = 0; i < m_guides.size(); ++i)
    {
        const auto& p = m_guides[i];

        Object o;
        o.Init();
        o.AddTexture(m_guideTex);
        o.SetPos(p.x, p.y, 0.0f);
        o.SetSize(m_guideSize*3, m_guideSize*3, 0.0f);

        // 必要なら到達済みを薄くするなど（任意）
        if ((int)i < m_guideIndex) o.SetColor(1,0,0,0.4);

        m_guideObjects.push_back(o);
    }
}

void RemoteWindPin::DrawGuides()
{
    if (!m_drawGuides) return;
    if (m_guideObjects.empty()) return;

    for (auto& o : m_guideObjects)
    {
        o.Draw(
            g_pDeviceContext,
            g_pInputLayout,
            g_pVertexShader,
            g_pPixelShader,
            g_pConstantBuffer
        );
    }
}

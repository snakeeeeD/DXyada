//#include "DecorMarkSystem.h"
//
//// 外部に必要なら（あなたの描画呼び出しに合わせる）
//extern ID3D11DeviceContext* g_pDeviceContext;
//extern ID3D11InputLayout* g_pInputLayout;
//extern ID3D11VertexShader* g_pVertexShader;
//extern ID3D11PixelShader* g_pPixelShader;
//extern ID3D11Buffer* g_pConstantBuffer;
//
//DecorMarkSystem::DecorMarkSystem()
//    : m_enabled(true)
//{
//}
//
//DecorMarkSystem::~DecorMarkSystem()
//{
//    Clear();
//}
//
//void DecorMarkSystem::Init()
//{
//    // デフォルト（あなたの希望：後から差し替え前提で統一も可）
//    // いったん全部 back.png でも動く
//    m_texRibbon = "asset/Field/back.png";
//    m_texWarning = "asset/Field/back.png";
//    m_texAnchor = "asset/Field/back.png";
//}
//
//void DecorMarkSystem::SetTextureFor(DecorMarkType type, const std::string& path)
//{
//    if (type == DecorMarkType::Ribbon)  m_texRibbon = path;
//    if (type == DecorMarkType::Warning) m_texWarning = path;
//    if (type == DecorMarkType::Anchor)  m_texAnchor = path;
//}
//
//const char* DecorMarkSystem::GetTexturePath(DecorMarkType type) const
//{
//    if (type == DecorMarkType::Ribbon)  return m_texRibbon.c_str();
//    if (type == DecorMarkType::Warning) return m_texWarning.c_str();
//    if (type == DecorMarkType::Anchor)  return m_texAnchor.c_str();
//    return nullptr;
//}
//
//DecorMarkType DecorMarkSystem::DecideMarkType(ColliderTag tag) const
//{
//    // ここを 1 箇所変えるだけでルール変更できる
//    switch (tag)
//    {
//    case ColliderTag::Pin:
//        return DecorMarkType::Ribbon;
//
//    case ColliderTag::Enemy:
//        return DecorMarkType::Warning;
//
//    case ColliderTag::Platform:
//        return DecorMarkType::Anchor;
//
//    default:
//        break;
//    }
//    return DecorMarkType::None;
//}
//
//void DecorMarkSystem::ApplyIconVisual(DecorMarker& m)
//{
//    const char* tex = GetTexturePath(m.type);
//    if (!tex) return;
//
//    // Object の初期化が済んでいない可能性があるので Init を呼ぶ設計
//    // すでに Init 済みなら Object::Init が二重で困る場合は Object 側に対応が必要
//    m.icon.Init();
//    m.icon.AddTexture(tex);
//    m.icon.SetSize(m.sizeW, m.sizeH, 0.0f);
//
//    // 色を変えたいならここ（あなたの Object::SetColor がある想定）
//    // m.icon.SetColor(1,1,1,1);
//}
//
//void DecorMarkSystem::FollowTarget(DecorMarker& m)
//{
//    if (!m.target) return;
//
//    const DirectX::XMFLOAT3 tpos = m.target->GetPos();
//    const DirectX::XMFLOAT3 tsz = m.target->GetSize();
//
//    // 頭上追従：対象の上端 + 少し上
//    float px = tpos.x + m.offsetX;
//    float py = tpos.y + (tsz.y * 0.5f) + m.offsetY;
//
//    m.icon.SetPos(px, py, 0.0f);
//}
//
//void DecorMarkSystem::AddMarker(Object* target, ColliderTag tag)
//{
//    if (!m_enabled) return;
//    if (!target) return;
//
//    // 既にあるなら何もしない（重複生成防止）
//    std::unordered_map<Object*, size_t>::iterator it = m_indexByTarget.find(target);
//    if (it != m_indexByTarget.end())
//    {
//        // 既存を更新するならここで tag/type を更新
//        size_t idx = it->second;
//        if (idx < m_markers.size())
//        {
//            DecorMarker& mk = m_markers[idx];
//            mk.tag = tag;
//            mk.type = DecideMarkType(tag);
//            ApplyIconVisual(mk);
//            mk.active = true;
//        }
//        return;
//    }
//
//    DecorMarker m;
//    m.target = target;
//    m.tag = tag;
//    m.type = DecideMarkType(tag);
//
//    if (m.type == DecorMarkType::None)
//        return;
//
//    // サイズ・オフセット：固定でもよいが、扱いやすい値をデフォルトで入れる
//    m.sizeW = 32.0f;
//    m.sizeH = 32.0f;
//
//    m.offsetX = 0.0f;
//    m.offsetY = 12.0f; // “少し上” の分
//
//    ApplyIconVisual(m);
//    FollowTarget(m);
//
//    m.active = true;
//
//    m_markers.push_back(m);
//    m_indexByTarget[target] = m_markers.size() - 1;
//}
//
//void DecorMarkSystem::RemoveMarker(Object* target)
//{
//    if (!target) return;
//
//    std::unordered_map<Object*, size_t>::iterator it = m_indexByTarget.find(target);
//    if (it == m_indexByTarget.end())
//        return;
//
//    size_t idx = it->second;
//    if (idx >= m_markers.size())
//    {
//        m_indexByTarget.erase(it);
//        return;
//    }
//
//    // vector なので swap-pop で O(1) 削除
//    size_t last = m_markers.size() - 1;
//    if (idx != last)
//    {
//        // idx と last を交換
//        DecorMarker tmp = m_markers[idx];
//        m_markers[idx] = m_markers[last];
//        m_markers[last] = tmp;
//
//        // index map を更新（swap された方）
//        Object* swappedTarget = m_markers[idx].target;
//        if (swappedTarget)
//        {
//            m_indexByTarget[swappedTarget] = idx;
//        }
//    }
//
//    // 末尾を削除
//    m_markers.pop_back();
//    m_indexByTarget.erase(target);
//}
//
//void DecorMarkSystem::Clear()
//{
//    m_markers.clear();
//    m_indexByTarget.clear();
//}
//
//void DecorMarkSystem::Update(float dt)
//{
//    if (!m_enabled) return;
//
//    // 追従更新
//    for (size_t i = 0; i < m_markers.size(); ++i)
//    {
//        DecorMarker& mk = m_markers[i];
//        if (!mk.active) continue;
//        if (!mk.target)
//        {
//            mk.active = false;
//            continue;
//        }
//
//        FollowTarget(mk);
//        mk.icon.Update(dt);
//    }
//
//    // 無効化されたものを掃除したければここで RemoveMarker する
//    // ただし target=nullptr にならない限り安全で、現状は手動削除推奨
//}
//
//void DecorMarkSystem::Draw()
//{
//    if (!m_enabled) return;
//
//    for (size_t i = 0; i < m_markers.size(); ++i)
//    {
//        DecorMarker& mk = m_markers[i];
//        if (!mk.active) continue;
//
//        mk.icon.Draw(
//            g_pDeviceContext,
//            g_pInputLayout,
//            g_pVertexShader,
//            g_pPixelShader,
//            g_pConstantBuffer
//        );
//    }
//}
//
//// ----- IDecorEventListener -----
//
//void DecorMarkSystem::OnDecorated(Object* target, ColliderTag tag)
//{
//    AddMarker(target, tag);
//}
//
//void DecorMarkSystem::OnUndecorated(Object* target)
//{
//    RemoveMarker(target);
//}
//
//void DecorMarkSystem::OnTargetDestroyed(Object* target)
//{
//    // 対象が破棄される直前に呼べるなら、これが一番安全
//    RemoveMarker(target);
//}

#include "DecorMarkSystem.h"

// あなたの Object::Draw 呼び出しに合わせた extern 群
extern ID3D11DeviceContext* g_pDeviceContext;
extern ID3D11InputLayout* g_pInputLayout;
extern ID3D11VertexShader* g_pVertexShader;
extern ID3D11PixelShader* g_pPixelShader;
extern ID3D11Buffer* g_pConstantBuffer;

DecorMarkSystem::DecorMarkSystem()
    : m_enabled(true)
{
}

DecorMarkSystem::~DecorMarkSystem()
{
    Clear();
}

void DecorMarkSystem::Init()
{
    // まず全部同じで動かす（あとで差し替え可）
    m_texRibbon = "asset/Field/back.png";
    m_texWarning = "asset/Field/back.png";
    m_texAnchor = "asset/Field/back.png";
}

void DecorMarkSystem::SetTextureFor(DecorMarkType type, const std::string& path)
{
    if (type == DecorMarkType::Ribbon)  m_texRibbon = path;
    if (type == DecorMarkType::Warning) m_texWarning = path;
    if (type == DecorMarkType::Anchor)  m_texAnchor = path;
}

const char* DecorMarkSystem::GetTexturePath(DecorMarkType type) const
{
    if (type == DecorMarkType::Ribbon)  return m_texRibbon.c_str();
    if (type == DecorMarkType::Warning) return m_texWarning.c_str();
    if (type == DecorMarkType::Anchor)  return m_texAnchor.c_str();
    return nullptr;
}

DecorMarkType DecorMarkSystem::DecideMarkType(ColliderTag tag) const
{
    // あなたの ColliderTag の実態に合わせる
    // ここは tag の enum 名が違うなら修正してOK
    switch (tag)
    {
    case ColliderTag::Pin:
        return DecorMarkType::Ribbon;
    case ColliderTag::Enemy:
        return DecorMarkType::Warning;
    case ColliderTag::Platform:
        return DecorMarkType::Anchor;
    default:
        // 想定外は Ribbon 扱いにしない（作らない）
        // AddMarker 側で弾くための “ダミー” を返す手もあるが、ここでは Ribbon にしない
        return DecorMarkType::Ribbon; // ※AddMarker側で default を弾くならここは到達しない
    }
}

void DecorMarkSystem::ApplyIconVisual(DecorMarker& m)
{
    const char* tex = GetTexturePath(m.type);
    if (!tex) return;

    // Object の仕様が「Init() を毎回呼んでも良い」前提
    // もしダメなら icon.Init() を一度だけにする必要がある
    m.icon.Init();
    m.icon.AddTexture(tex);
    m.icon.SetSize(m.sizeW, m.sizeH, 0.0f);
}

void DecorMarkSystem::FollowTarget(DecorMarker& m)
{
    if (!m.target) return;

    const DirectX::XMFLOAT3 tpos = m.target->GetPos();
    const DirectX::XMFLOAT3 tsz = m.target->GetSize();

    float px = tpos.x + m.offsetX;
    float py = tpos.y + (tsz.y * 0.5f) + m.offsetY;

    m.icon.SetPos(px, py, 0.0f);
}

void DecorMarkSystem::AddMarker(Object* target, ColliderTag tag)
{
    if (!m_enabled) return;
    if (!target) return;

    // “作る対象か”を先に判定（default は作らない）
    DecorMarkType type;
    bool ok = true;

    // tag の種類が合わない場合ここで弾く
    if (tag == ColliderTag::Pin) type = DecorMarkType::Ribbon;
    else if (tag == ColliderTag::Enemy) type = DecorMarkType::Warning;
    else if (tag == ColliderTag::Platform) type = DecorMarkType::Anchor;
    else ok = false;

    if (!ok) return;

    std::unordered_map<Object*, size_t>::iterator it = m_indexByTarget.find(target);
    if (it != m_indexByTarget.end())
    {
        size_t idx = it->second;
        if (idx < m_markers.size())
        {
            DecorMarker& mk = m_markers[idx];
            mk.tag = tag;
            mk.type = type;
            mk.active = true;
            ApplyIconVisual(mk);
            FollowTarget(mk);
        }
        return;
    }

    DecorMarker m;
    m.target = target;
    m.tag = tag;
    m.type = type;

    m.sizeW = 32.0f;
    m.sizeH = 32.0f;

    m.offsetX = 0.0f;
    m.offsetY = 12.0f;

    ApplyIconVisual(m);
    FollowTarget(m);

    m.active = true;

    m_markers.push_back(m);
    m_indexByTarget[target] = m_markers.size() - 1;
}

void DecorMarkSystem::RemoveMarker(Object* target)
{
    if (!target) return;

    std::unordered_map<Object*, size_t>::iterator it = m_indexByTarget.find(target);
    if (it == m_indexByTarget.end()) return;

    size_t idx = it->second;
    if (idx >= m_markers.size())
    {
        m_indexByTarget.erase(it);
        return;
    }

    size_t last = m_markers.size() - 1;
    if (idx != last)
    {
        DecorMarker tmp = m_markers[idx];
        m_markers[idx] = m_markers[last];
        m_markers[last] = tmp;

        Object* swappedTarget = m_markers[idx].target;
        if (swappedTarget)
            m_indexByTarget[swappedTarget] = idx;
    }

    m_markers.pop_back();
    m_indexByTarget.erase(target);
}

void DecorMarkSystem::Clear()
{
    m_markers.clear();
    m_indexByTarget.clear();
}

void DecorMarkSystem::Update(float dt)
{


    if (!m_enabled) return;

    for (size_t i = 0; i < m_markers.size(); ++i)
    {
        DecorMarker& mk = m_markers[i];
        if (!mk.active) continue;
        if (!mk.target)
        {
            mk.active = false;
            continue;
        }

        FollowTarget(mk);
        mk.icon.Update(dt);
    }
}

void DecorMarkSystem::Draw()
{
    if (!m_enabled) return;

    for (size_t i = 0; i < m_markers.size(); ++i)
    {
        DecorMarker& mk = m_markers[i];
        if (!mk.active) continue;

        mk.icon.Draw(
            g_pDeviceContext,
            g_pInputLayout,
            g_pVertexShader,
            g_pPixelShader,
            g_pConstantBuffer
        );
    }
}

void DecorMarkSystem::OnDecorated(Object* target, ColliderTag tag)
{
    AddMarker(target, tag);
}

void DecorMarkSystem::OnUndecorated(Object* target)
{
    RemoveMarker(target);
}

void DecorMarkSystem::OnTargetDestroyed(Object* target)
{
    RemoveMarker(target);
}

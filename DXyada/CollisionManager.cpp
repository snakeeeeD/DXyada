#include "CollisionManager.h"

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>           

#include "SceneManager.h"   // 必要なら。要らないなら消してOK

using namespace DirectX;

// ===== 内部ユーティリティ =====
void CollisionManager::RemoveFromVec(std::vector<Object*>& v, Object* obj)
{
    v.erase(std::remove(v.begin(), v.end(), obj), v.end());
}

void CollisionManager::RemoveFromByTag(Object* obj, ColliderTag tag)
{
    // ★ ここが崩れやすいので >> を避ける
    typedef std::unordered_map<ColliderTag, std::vector<Object*> > TagMap;
    TagMap::iterator it = m_byTag.find(tag);
    if (it == m_byTag.end()) return;

    std::vector<Object*>& vec = it->second;
    vec.erase(std::remove(vec.begin(), vec.end(), obj), vec.end());
}

// ===== 登録 =====
void CollisionManager::AddDynamic(Object* obj)
{
    if (!obj) return;
    m_dynamic.push_back(obj);
}

void CollisionManager::AddStatic(Object* obj)
{
    if (!obj) return;
    m_static.push_back(obj);
}

void CollisionManager::AddMoved(Object* obj)
{
    if (!obj) return;
    m_moved.push_back(obj);
}

// ===== Tag 管理 =====
void CollisionManager::SetTag(Object* obj, ColliderTag tag)
{
    if (!obj) return;

    std::unordered_map<Object*, ColliderTag>::iterator it = m_tags.find(obj);

    if (it != m_tags.end())
    {
        ColliderTag oldTag = it->second;
        if (oldTag == tag) return;

        RemoveFromByTag(obj, oldTag);
        it->second = tag;
    }
    else
    {
        m_tags[obj] = tag;
    }

    // newTag 側へ追加（重複防止）
    std::vector<Object*>& v = m_byTag[tag];
    if (std::find(v.begin(), v.end(), obj) == v.end())
    {
        v.push_back(obj);
    }
}

ColliderTag CollisionManager::GetTag(Object* obj) const
{
    std::unordered_map<Object*, ColliderTag>::const_iterator it = m_tags.find(obj);
    if (it != m_tags.end()) return it->second;
    return ColliderTag::Unknown;
}

const std::vector<Object*>& CollisionManager::GetObjectsByTag(ColliderTag tag) const
{
    static const std::vector<Object*> s_empty;

    typedef std::unordered_map<ColliderTag, std::vector<Object*> > TagMap;
    TagMap::const_iterator it = m_byTag.find(tag);
    if (it == m_byTag.end()) return s_empty;

    return it->second;
}

// ===== AABB =====
CollisionManager::AABB CollisionManager::GetAABB(Object* obj)
{
    XMFLOAT3 pos = obj->GetPos();
    XMFLOAT3 size = obj->GetSize();

    float halfW = size.x * 0.5f;
    float halfH = size.y * 0.5f;

    AABB box;
    box.min = XMFLOAT2(pos.x - halfW, pos.y - halfH);
    box.max = XMFLOAT2(pos.x + halfW, pos.y + halfH);
    return box;
}

bool CollisionManager::CheckOverlap(const AABB& a, const AABB& b)
{
    if (a.max.x < b.min.x) return false;
    if (a.min.x > b.max.x) return false;
    if (a.max.y < b.min.y) return false;
    if (a.min.y > b.max.y) return false;
    return true;
}

DirectX::XMFLOAT2 CollisionManager::GetMTV(const AABB& a, const AABB& b)
{
    float dx1 = b.max.x - a.min.x;
    float dx2 = b.min.x - a.max.x;
    float dy1 = b.max.y - a.min.y;
    float dy2 = b.min.y - a.max.y;

    float mtvX = (std::fabs(dx1) < std::fabs(dx2)) ? dx1 : dx2;
    float mtvY = (std::fabs(dy1) < std::fabs(dy2)) ? dy1 : dy2;

    if (std::fabs(mtvX) < std::fabs(mtvY))
        return XMFLOAT2(mtvX, 0.0f);
    else
        return XMFLOAT2(0.0f, mtvY);
}

// ===== 判定本体（あなたの現状を維持） =====
void CollisionManager::CheckAll()
{
    // Dynamic vs Static
    for (size_t i = 0; i < m_dynamic.size(); ++i)
    {
        Object* dynObj = m_dynamic[i];
        if (!dynObj) continue;

        AABB a = GetAABB(dynObj);

        for (size_t j = 0; j < m_static.size(); ++j)
        {
            Object* stObj = m_static[j];
            if (!stObj) continue;

            AABB b = GetAABB(stObj);

            ColliderTag tagA = GetTag(dynObj);
            ColliderTag tagB = GetTag(stObj);

            if (!CheckOverlap(a, b)) continue;

            if (tagA == ColliderTag::Player && tagB == ColliderTag::Enemy)
            {
                if (m_sceneMgr)
                {
                    // 必要な処理があればここ
                }
                continue;
            }

            XMFLOAT2 mtv = GetMTV(a, b);

            XMFLOAT3 pos = dynObj->GetPos();
            pos.x += mtv.x;
            pos.y += mtv.y;
            dynObj->SetPos(pos.x, pos.y, pos.z);

            a = GetAABB(dynObj);
        }
    }

    // Dynamic vs Moved
    for (size_t i = 0; i < m_dynamic.size(); ++i)
    {
        Object* dynObj = m_dynamic[i];
        if (!dynObj) continue;

        AABB a = GetAABB(dynObj);

        for (size_t j = 0; j < m_moved.size(); ++j)
        {
            Object* moveObj = m_moved[j];
            if (!moveObj) continue;

            AABB b = GetAABB(moveObj);

            ColliderTag tagA = GetTag(dynObj);
            ColliderTag tagB = GetTag(moveObj);

            if (!CheckOverlap(a, b)) continue;

            if (tagA == ColliderTag::Player && tagB == ColliderTag::Enemy)
            {
                if (m_playerInvincible) continue;
            }

            XMFLOAT2 mtv = GetMTV(a, b);

            XMFLOAT3 posA = dynObj->GetPos();
            XMFLOAT3 posB = moveObj->GetPos();

            posA.x += mtv.x * 0.5f;
            posA.y += mtv.y * 0.5f;
            posB.x -= mtv.x * 0.5f;
            posB.y -= mtv.y * 0.5f;

            dynObj->SetPos(posA.x, posA.y, posA.z);
            moveObj->SetPos(posB.x, posB.y, posB.z);

            a = GetAABB(dynObj);
        }
    }

    // Moved vs Moved
    for (size_t i = 0; i < m_moved.size(); ++i)
    {
        for (size_t j = i + 1; j < m_moved.size(); ++j)
        {
            Object* Aobj = m_moved[i];
            Object* Bobj = m_moved[j];
            if (!Aobj || !Bobj) continue;

            AABB a = GetAABB(Aobj);
            AABB b = GetAABB(Bobj);

            if (!CheckOverlap(a, b)) continue;

            XMFLOAT2 mtv = GetMTV(a, b);

            XMFLOAT3 posA = Aobj->GetPos();
            XMFLOAT3 posB = Bobj->GetPos();

            posA.x += mtv.x * 0.5f;
            posA.y += mtv.y * 0.5f;
            posB.x -= mtv.x * 0.5f;
            posB.y -= mtv.y * 0.5f;

            Aobj->SetPos(posA.x, posA.y, posA.z);
            Bobj->SetPos(posB.x, posB.y, posB.z);
        }
    }

    // Moved vs Static
    for (size_t i = 0; i < m_moved.size(); ++i)
    {
        Object* move = m_moved[i];
        if (!move) continue;

        AABB a = GetAABB(move);

        for (size_t j = 0; j < m_static.size(); ++j)
        {
            Object* stObj = m_static[j];
            if (!stObj) continue;

            AABB b = GetAABB(stObj);

            if (!CheckOverlap(a, b)) continue;

            XMFLOAT2 mtv = GetMTV(a, b);

            XMFLOAT3 pos = move->GetPos();
            pos.x += mtv.x;
            pos.y += mtv.y;
            move->SetPos(pos.x, pos.y, pos.z);

            a = GetAABB(move);
        }
    }
}

// ===== Hit チェック =====
bool CollisionManager::CheckHitStatic(const AABB& box)
{
    for (size_t i = 0; i < m_static.size(); ++i)
    {
        Object* stObj = m_static[i];
        if (!stObj) continue;

        AABB b = GetAABB(stObj);
        if (CheckOverlap(box, b)) return true;
    }
    return false;
}

bool CollisionManager::CheckHitMoved(const AABB& box)
{
    for (size_t i = 0; i < m_moved.size(); ++i)
    {
        Object* mvObj = m_moved[i];
        if (!mvObj) continue;

        AABB b = GetAABB(mvObj);
        if (CheckOverlap(box, b)) return true;
    }
    return false;
}

// ===== Remove =====
void CollisionManager::Remove(Object* obj)
{
    if (!obj) return;

    RemoveFromVec(m_static, obj);
    RemoveFromVec(m_dynamic, obj);
    RemoveFromVec(m_moved, obj);

    std::unordered_map<Object*, ColliderTag>::iterator it = m_tags.find(obj);
    if (it != m_tags.end())
    {
        ColliderTag oldTag = it->second;
        RemoveFromByTag(obj, oldTag);
        m_tags.erase(it);
    }
}

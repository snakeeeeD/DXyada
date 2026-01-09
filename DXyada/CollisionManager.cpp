#include "CollisionManager.h"
#include <algorithm>
#include"Stage1.h"
#include"Stage2.h"
#include "SceneManager.h"

using namespace DirectX;

void CollisionManager::AddDynamic(Object* obj) 
{
    m_dynamic.push_back(obj);
}

void CollisionManager::AddStatic(Object* obj) 
{
    m_static.push_back(obj);
}

void CollisionManager::AddMoved(Object* obj) 
{
    m_moved.push_back(obj);
}

CollisionManager::AABB CollisionManager::GetAABB(Object* obj)
{
    XMFLOAT3 pos = obj->GetPos();     // 中心座標
    XMFLOAT3 size = obj->GetSize();   // 幅・高さ

    float halfW = size.x * 0.5f;
    float halfH = size.y * 0.5f;

    AABB box;
    box.min = { pos.x - halfW, pos.y - halfH };
    box.max = { pos.x + halfW, pos.y + halfH };

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


XMFLOAT2 CollisionManager::GetMTV(const AABB& a, const AABB& b)
{
    float dx1 = b.max.x - a.min.x;
    float dx2 = b.min.x - a.max.x;
    float dy1 = b.max.y - a.min.y;
    float dy2 = b.min.y - a.max.y;

    float mtvX = fabs(dx1) < fabs(dx2) ? dx1 : dx2;
    float mtvY = fabs(dy1) < fabs(dy2) ? dy1 : dy2;

    if (fabs(mtvX) < fabs(mtvY)) 
    {
        return XMFLOAT2(mtvX, 0);
    }
    else 
    {
        return XMFLOAT2(0, mtvY);
    }
}


/*
* 当たり判定はすべてここで管理
*/
void CollisionManager::CheckAll()
{


    for (auto* dynObj : m_dynamic) 
    {

        AABB a = GetAABB(dynObj);   //dynObj：動的なオブジェクト（dynamic）

        for (auto* stObj : m_static) {

            AABB b = GetAABB(stObj);    //stObj：静的なオブジェクト（static）

            ColliderTag tagA = GetTag(dynObj);  //tagAを動的オブジェクト
            ColliderTag tagB = GetTag(stObj);   //tagBを静的オブジェクト

            if (!CheckOverlap(a, b)) continue;

            //Aがプレイヤー、BがEnemyの場合
            if (tagA == ColliderTag::Player && tagB == ColliderTag::Enemy) 
            {
                if (m_sceneMgr) {
                }
                continue;
            }

            // MTV 計算して押し戻し
            XMFLOAT2 mtv = GetMTV(a, b);

            XMFLOAT3 pos = dynObj->GetPos();
            pos.x += mtv.x;
            pos.y += mtv.y;
            dynObj->SetPos(pos.x, pos.y, pos.z);
        }
    }

    for (auto* dynObj : m_dynamic) 
    {

        AABB a = GetAABB(dynObj);
        
        for (auto* moveObj : m_moved) 
        {

            AABB b = GetAABB(moveObj);
        
            ColliderTag tagA = GetTag(dynObj);
            ColliderTag tagB = GetTag(moveObj);

            if (!CheckOverlap(a, b)) continue;

            if (tagA == ColliderTag::Player && tagB == ColliderTag::Enemy)
            {
                if (m_playerInvincible)
                {
                    continue;  // 無敵時はすり抜け
                }
            }

            XMFLOAT2 mtv = GetMTV(a, b);
            DirectX::XMFLOAT3 posA = dynObj->GetPos();
            DirectX::XMFLOAT3 posB = moveObj->GetPos();
            posA.x += mtv.x * 0.5f;
            posA.y += mtv.y * 0.5f;
            posB.x -= mtv.x * 0.5f;
            posB.y -= mtv.y * 0.5f;
            dynObj->SetPos(posA.x, posA.y, posA.z);
            moveObj->SetPos(posB.x, posB.y, posB.z);
        }
    }

    // 移動可能同士
    size_t count = m_moved.size();
    for (size_t i = 0; i < count; ++i) 
    {
        for (size_t j = i + 1; j < count; ++j) 
        {
            AABB a = GetAABB(m_moved[i]);
            AABB b = GetAABB(m_moved[j]);
            if (!CheckOverlap(a, b)) continue;
            XMFLOAT2 mtv = GetMTV(a, b);
            DirectX::XMFLOAT3 posA = m_moved[i]->GetPos();
            DirectX::XMFLOAT3 posB = m_moved[j]->GetPos();
            posA.x += mtv.x * 0.5f;
            posA.y += mtv.y * 0.5f;
            posB.x -= mtv.x * 0.5f;
            posB.y -= mtv.y * 0.5f;
            m_moved[i]->SetPos(posA.x, posA.y, posA.z);
            m_moved[j]->SetPos(posB.x, posB.y, posB.z);
        }
    }

    for (auto* move : m_moved) 
    {

        AABB a = GetAABB(move);

        for (auto* stObj : m_static) 
        {

            AABB b = GetAABB(stObj);

            if (!CheckOverlap(a, b)) continue;

            // MTV 計算して押し戻し
            XMFLOAT2 mtv = GetMTV(a, b);

            XMFLOAT3 pos = move->GetPos();
            pos.x += mtv.x;
            pos.y += mtv.y;
            move->SetPos(pos.x, pos.y, pos.z);
        }
    }
}

bool CollisionManager::CheckHitStatic(const AABB& box)
{
    //登録されている全ての静的オブジェクトと判定
    for (auto* stObj : m_static)
    {
        AABB b = GetAABB(stObj);

        if (CheckOverlap(box, b))
        {
            return true;//どれか1つでも当たれば true
        }
    }
    return false; //何にも当たらなければ false
}

void CollisionManager::Remove(Object* obj)
{
    auto removeFrom = [&](std::vector<Object*>& list)
        {
            list.erase(
                std::remove(list.begin(), list.end(), obj),
                list.end()
            );
        };

    removeFrom(m_static);
    removeFrom(m_dynamic);
    removeFrom(m_moved);

    // Tag情報削除
    m_tags.erase(obj);
}

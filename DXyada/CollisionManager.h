///*
////#pragma once
////#include <vector>
////#include <unordered_map>
////#include <DirectXMath.h>
////#include "Object.h"
////
////enum class ColliderTag {
////    Player,
////    Enemy,
////    Platform,
////    Unknown
////};
////
////class CollisionManager {
////private:
////
////    struct AABB {
////        DirectX::XMFLOAT2 min;
////        DirectX::XMFLOAT2 max;
////    };
////
////    std::vector<Object*> m_dynamic;
////    std::vector<Object*> m_static;
////    std::vector<Object*> m_moved;
////
////    AABB GetAABB(Object* obj);
////    bool CheckOverlap(const AABB& a, const AABB& b);
////    DirectX::XMFLOAT2 GetMTV(const AABB& a, const AABB& b);
////
////    std::unordered_map<Object*, ColliderTag> m_tags;
////
////    class SceneManager;
////public:
////    // 登録
////    void AddDynamic(Object* obj);
////    void AddStatic(Object* obj);
////    void AddMoved(Object* obj);
////
////    // 毎フレームチェック
////    void CheckAll();
////
////    void SetTag(Object* obj, ColliderTag tag) {
////        m_tags[obj] = tag;
////    }
////
////    ColliderTag GetTag(Object* obj) {
////        auto it = m_tags.find(obj);
////        if (it != m_tags.end()) return it->second;
////        return ColliderTag::Unknown;
////    }
////
////    void SetSceneManager(SceneManager* mgr) { m_sceneMgr = mgr; }
////};
//*/
//
//#pragma once
//#include <vector>
//#include <unordered_map>
//#include <DirectXMath.h>
//#include "Object.h"
//
//// forward 宣言（重要）
//class SceneManager;
//class Object;
//
//enum class ColliderTag 
//{
//    Player,
//    Enemy,
//    Platform,
//    Pin,
//    Unknown
//};
//
//class CollisionManager 
//{
//public:
//    struct AABB
//    {
//        DirectX::XMFLOAT2 min;
//        DirectX::XMFLOAT2 max;
//    };
//
//
//private:
//
//    std::vector<Object*> m_dynamic;
//    std::vector<Object*> m_static;
//    std::vector<Object*> m_moved;
//
//    std::unordered_map<Object*, ColliderTag> m_tags;
//
//    std::vector<AABB> m_staticObjects;
//
//
//    SceneManager* m_sceneMgr = nullptr;
//
//    bool m_playerInvincible = false;
//
// 
//    DirectX::XMFLOAT2 GetMTV(const AABB& a, const AABB& b);
//
//    // Object* -> Tag
//    std::unordered_map<Object*, ColliderTag> m_tags;
//
//    // Tag -> Objects
//    std::unordered_map<ColliderTag, std::vector<Object*>> m_byTag;
//
//
//public:
//
//    void AddDynamic(Object* obj);
//    void AddStatic(Object* obj);
//    void AddMoved(Object* obj);
//
//    void CheckAll();
//
//    void Remove(Object* obj);
//
//    void SetTag(Object* obj, ColliderTag tag) 
//    {
//        m_tags[obj] = tag;
//    }
//
//    ColliderTag GetTag(Object* obj) 
//    {
//        auto it = m_tags.find(obj);
//        if (it != m_tags.end()) return it->second;
//        return ColliderTag::Unknown;
//    }
//
//    void SetSceneManager(SceneManager* mgr) { m_sceneMgr = mgr; }
//
//    AABB GetAABB(Object* obj);
//    bool CheckOverlap(const AABB& a, const AABB& b);
//
//    //指定したAABB(矩形)がStaticオブジェクトと当たっているか調べる
//    bool CheckHitStatic(const AABB& box);
//    bool CheckHitMoved(const AABB& box);
//
//    void AddStaticAABB(const AABB& aabb) 
//    {
//        m_staticObjects.push_back(aabb);
//    }
//
//    void SetPlayerInvincible(bool invincible) { m_playerInvincible = invincible; }
//    bool IsPlayerInvincible() const { return m_playerInvincible; }
//
//};

#pragma once
#include <vector>
#include <unordered_map>
#include <algorithm>        // remove, find
#include <DirectXMath.h>
#include "Object.h"

// forward 宣言
class SceneManager;

enum class ColliderTag
{
    Player,
    Enemy,
    Platform,
    Pin,
    Unknown
};

class CollisionManager
{
public:
    struct AABB
    {
        DirectX::XMFLOAT2 min;
        DirectX::XMFLOAT2 max;
    };

private:
    std::vector<Object*> m_dynamic;
    std::vector<Object*> m_static;
    std::vector<Object*> m_moved;

    // AABB を別管理しているなら残す（あなたの既存設計を維持）
    std::vector<AABB> m_staticObjects;

    SceneManager* m_sceneMgr = nullptr;

    bool m_playerInvincible = false;

    // Object* -> Tag（※重複定義しない）
    std::unordered_map<Object*, ColliderTag> m_tags;

    // Tag -> Objects（方針Aの核）
    std::unordered_map<ColliderTag, std::vector<Object*>> m_byTag;

private:
    DirectX::XMFLOAT2 GetMTV(const AABB& a, const AABB& b);

    // 内部ユーティリティ
    void RemoveFromVec(std::vector<Object*>& v, Object* obj);
    void RemoveFromByTag(Object* obj, ColliderTag tag);

public:
    // 登録
    void AddDynamic(Object* obj);
    void AddStatic(Object* obj);
    void AddMoved(Object* obj);

    // 毎フレームチェック
    void CheckAll();

    // 削除（登録解除）
    void Remove(Object* obj);

    // Tag
    void SetTag(Object* obj, ColliderTag tag);
    ColliderTag GetTag(Object* obj) const;

    // Tag から一覧取得（Playerがこれを使う）
    const std::vector<Object*>& GetObjectsByTag(ColliderTag tag) const;

    void SetSceneManager(SceneManager* mgr) { m_sceneMgr = mgr; }

    // AABB
    AABB GetAABB(Object* obj);
    bool CheckOverlap(const AABB& a, const AABB& b);

    bool CheckHitStatic(const AABB& box);
    bool CheckHitMoved(const AABB& box);

    void AddStaticAABB(const AABB& aabb)
    {
        m_staticObjects.push_back(aabb);
    }

    void SetPlayerInvincible(bool invincible) { m_playerInvincible = invincible; }
    bool IsPlayerInvincible() const { return m_playerInvincible; }
};

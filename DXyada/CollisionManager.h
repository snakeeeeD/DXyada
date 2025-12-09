//#pragma once
//#include <vector>
//#include <unordered_map>
//#include <DirectXMath.h>
//#include "Object.h"
//
//enum class ColliderTag {
//    Player,
//    Enemy,
//    Platform,
//    Unknown
//};
//
//class CollisionManager {
//private:
//
//    struct AABB {
//        DirectX::XMFLOAT2 min;
//        DirectX::XMFLOAT2 max;
//    };
//
//    std::vector<Object*> m_dynamic;
//    std::vector<Object*> m_static;
//    std::vector<Object*> m_moved;
//
//    AABB GetAABB(Object* obj);
//    bool CheckOverlap(const AABB& a, const AABB& b);
//    DirectX::XMFLOAT2 GetMTV(const AABB& a, const AABB& b);
//
//    std::unordered_map<Object*, ColliderTag> m_tags;
//
//    class SceneManager;
//public:
//    // 登録
//    void AddDynamic(Object* obj);   
//    void AddStatic(Object* obj);    
//    void AddMoved(Object* obj);   
//
//    // 毎フレームチェック
//    void CheckAll();
//
//    void SetTag(Object* obj, ColliderTag tag) {
//        m_tags[obj] = tag;
//    }
//
//    ColliderTag GetTag(Object* obj) {
//        auto it = m_tags.find(obj);
//        if (it != m_tags.end()) return it->second;
//        return ColliderTag::Unknown;
//    }
//
//    void SetSceneManager(SceneManager* mgr) { m_sceneMgr = mgr; }
//};


#pragma once
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
#include "Object.h"

// forward 宣言（重要）
class SceneManager;

enum class ColliderTag {
    Player,
    Enemy,
    Platform,
    Unknown
};

class CollisionManager {
private:

    struct AABB {
        DirectX::XMFLOAT2 min;
        DirectX::XMFLOAT2 max;
    };

    std::vector<Object*> m_dynamic;
    std::vector<Object*> m_static;
    std::vector<Object*> m_moved;

    std::unordered_map<Object*, ColliderTag> m_tags;


    SceneManager* m_sceneMgr = nullptr;

    AABB GetAABB(Object* obj);
    bool CheckOverlap(const AABB& a, const AABB& b);
    DirectX::XMFLOAT2 GetMTV(const AABB& a, const AABB& b);

public:
    void AddDynamic(Object* obj);
    void AddStatic(Object* obj);
    void AddMoved(Object* obj);

    void CheckAll();

    void SetTag(Object* obj, ColliderTag tag) {
        m_tags[obj] = tag;
    }

    ColliderTag GetTag(Object* obj) {
        auto it = m_tags.find(obj);
        if (it != m_tags.end()) return it->second;
        return ColliderTag::Unknown;
    }

    void SetSceneManager(SceneManager* mgr) { m_sceneMgr = mgr; }
};

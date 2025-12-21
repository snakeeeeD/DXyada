#pragma once
#include "Object.h"
#include "CollisionManager.h"

class Enemy 
{
protected:
    enum class EnamyState
    {
        Idle,
        Active,
        YouDied
    };
    Object m_object;
    CollisionManager* m_pCollision = nullptr;
    int m_direction = 1;
    float m_speed = 200.0f;

    float deltaTime;

    bool move = false;


    float m_velY = 0.0f;
    float m_gravity = 2000.0f;

    //強調表示用
    bool m_isHighlighted = false;
    bool m_enableCollision = true;
    EnamyState m_enemystate = EnamyState::Idle;
public:
    Tag tag = Tag::Enemy;

    Enemy() = default;
    virtual ~Enemy() = default;

    void Init();
    void Init(const char* texture, float x, float y, float width, float height);

    //CollisionManagerをセットする関数
    void SetCollisionManager(CollisionManager* col) { m_pCollision = col; }

    Object* GetObject() { return &m_object; }
    const Object* GetObject() const { return &m_object; }

    //強調表示用
    void SetHighlight(bool highlight) { m_isHighlighted = highlight; }
    bool IsHighLighted() const { m_isHighlighted; }

    virtual void Update(float deltaTime);
    virtual void Draw();
    void UnInit();

    void Disable();
    bool IsCollisionEnabled() const { return m_enableCollision; }
    bool IsDead() const { return m_enemystate == EnamyState::YouDied; }
};
#pragma once
#include "Object.h"
#include "CollisionManager.h"

class Player;

class Enemy
{
protected:

    Player* m_pPlayer = nullptr; // プレイヤーへのポインタ

    enum class EnemyState
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
    EnemyState m_enemystate = EnemyState::Idle;

    bool m_isSlow = false;

    bool m_isDecorated = false;          //飾られたかどうか
    bool m_hasChangedTexture = false;    //テクスチャ変更済みフラグ
    std::string m_decoratedTexturePath;  //飾られた時のテクスチャパス

    Object m_mark;
    Object m_mark2;
    Object m_mark3;
    bool   m_markVisible = true;

    float  m_markOffsetX = 0.0f;
    float  m_markOffsetY = 20.0f;
    float  m_markW = 320.0f;
    float  m_markH = 320.0f;

    bool m_decorated;
    bool m_justDecorated;

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

    Object* GetMarkObject() { return &m_mark; }
    Object* GetMarkObject2() { return &m_mark2; }
    Object* GetMarkObject3() { return &m_mark3; }

    //強調表示用
    void SetHighlight(bool highlight) { m_isHighlighted = highlight; }
    bool IsHighLighted() const { m_isHighlighted; }

    virtual void Update(float deltaTime);
    virtual void Draw();
    void UnInit();

    virtual void Disable(bool justdeco);
    bool IsCollisionEnabled() const { return m_enableCollision; }
    bool IsDead() const { return m_enemystate == EnemyState::YouDied; }

    bool m_isFrozen = false;

    void SetFrozen(bool flozen) { m_isFrozen = flozen; }
    bool IsFrozen() const { return m_isFrozen; }

    //プレイヤーをセットする関数
    void SetPlayer(Player* player) { m_pPlayer = player; }
    void SetSlow(bool slow) { m_isSlow = slow; }
    bool IsSlow() const { return m_isSlow; }

    virtual void OnDecorated();  //飾られた時の処理
    bool IsDecorated() const { return m_isDecorated; }

public:
    void NotifyDecoratedJust(bool just)
    {
        if (m_decorated) return;      // 1回だけ
        m_decorated = true;
        m_justDecorated = true;
        m_decoratedJust = just;       // “ジャスト演出”に使うなら保持
    }

    bool ConsumeJustDecorated()
    {
        if (!m_justDecorated) return false;
        m_justDecorated = false;
        return true;
    }

    bool WasJustDecorated() const { return m_decoratedJust; } // optional

private:
    bool m_decoratedJust = false; // optional

};
#pragma once
#include "Object.h"

class Enemy {
private:
    Object m_object;

    float deltaTime;

    bool move = false;


    float m_velY = 0.0f;
    float m_gravity = 2000.0f;
public:
    Tag tag = Tag::Enemy;

    Enemy() = default;
    ~Enemy() = default;

    void Init();
    void Init(const char* texture, float x, float y, float width, float height);

    Object* GetObject() { return &m_object; }
    const Object* GetObject() const { return &m_object; }

    void Update(float deltaTime);
    void Draw();
    void UnInit();
};

class EnemyBasic : public Enemy { /* 基本タイプ */ };
class EnemyFast : public Enemy { /* 速いタイプ */ };
class EnemyTank : public Enemy { /* HP高いタイプ */ };
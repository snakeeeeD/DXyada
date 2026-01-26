#pragma once
#include "Enemy.h"
#include "Texture.h"

class Rippa : public Enemy
{
public:
    enum class Type
    {
        Normal,     //オブジェクトに当たると
        Wandering
    };

private:

    Type m_type = Type::Normal; //デフォはノーマル

    //徘徊用のタイマー
    float m_turnTimer = 0.0f;
    float m_turnInterval = 3.0f;

    float m_walkspeed = 60.0f; //移動速度

    //センサーの厚み
    const float SENSOR_WIDTH = 15.0f;
    const float SENSOR_HEIGHT = 15.0f;

    int m_direction = -1; //向き
    //-1: 左
    // 1: 右

    float m_turnMinX = 0.0f;
    float m_turnMaxX = 0.0f;
    bool  m_useTurnRange = false;

public:
    Rippa(Type type = Type::Normal);
    //~Rippa() override;

    void Init(const char* tex, float x, float y, float w, float h);

    void SetType(Type type) { m_type = type; }
    void SetTurnInterval(float interval) { m_turnInterval = interval; }

    void SetTurnPos(float min, float max);

    void SetTurnRange(float min, float max);

    void Update(float deltaTime) override;

    //void Draw();

    void CheckCollision(); //Normal用
    void CheckWandering(float dt); // Wandering用

    void CheckEnemyCollision(const std::vector<Enemy*>& enemies);

};
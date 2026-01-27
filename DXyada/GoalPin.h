#pragma once
#include "Pin.h"
#include "Goal.h"

class GoalPin : public Pin
{

private:
    Goal m_goal;

public:
    GoalPin() = default;
    ~GoalPin() = default;

    void Init(
        const char* texture,
        float x,
        float y,
        float w,
        float h
    );

    void Update(float dt) override;
    void Draw() override;

    // ƒŠƒ{ƒ“‚ªŠª‚©‚ê‚½uŠÔ
    void OnDecorated();

    Goal* GetGoal() { return &m_goal; }
};
#include "GoalPin.h"

void GoalPin::Init(
    const char* texture,
    float x,
    float y,
    float w,
    float h
)
{
    // Pinとして初期化
    Pin::Init(texture, x, y, w, h);

    // ゴール専用設定
    SetcanDecorate(true);
    SetcanRollPin(false);
    SetPinKind(PinKind::Deco);

    // ピン自体は足場にしない
    SetEnablePlatformRegisterOnDecorated(false);

    // Goal 初期化
    m_goal.Init(
        "asset/Field/Goal.png",
        x,
        y,
        120,
        120
    );
}

void GoalPin::Update(float dt)
{
    Pin::Update(dt);
    auto pos = m_object.GetPos();
    m_goal.SetPos(pos.x, pos.y);
    // PinがDecoratedになったらGoalを解放
    if (GetState() == State::Decorated && !m_goal.IsDecorated())
    {
        m_goal.SetDecorated(true);
    }

}

void GoalPin::Draw()
{
    Pin::Draw();
    m_goal.Draw();
}

void GoalPin::OnDecorated()
{
    // ゴール解放
    m_goal.SetDecorated(true);

    // 見た目変化
    m_object.AddTexture("asset/Field/DecGoal.png");
}
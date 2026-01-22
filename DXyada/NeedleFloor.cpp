#include "NeedleFloor.h"

void NeedleFloor::Update(float deltaTime) 
{
    // ハリ床は移動しないので座標更新のみ
    m_object.Update(deltaTime);
}

void NeedleFloor::OnDecorated() 
{
    //OutputDebugStringA("aaaaaaaaaaaaaaaaaaaaaaaaaa\n");

    // すでに飾られていたら何もしない
    if (m_state == State::Decorated)
    {
        return;
    }

    m_state = State::Decorated;

    // 1. 見た目をリボン付きの画像に変更する（画像がある場合）
    // m_object.SetTexture("asset/needle_floor_ribbon.png");

    // 2. 【重要】CollisionManagerに「静止した地形」として登録する
    // これにより、PlayerやRippaのCheckHitStaticで「床」として認識されるようになります
    if (m_pCollision) 
    {
        // 自分の現在の座標とサイズから、その場で AABB を作成する
        auto pos = m_object.GetPos();
        auto size = m_object.GetSize();
        float hw = size.x * 0.5f;
        float hh = size.y * 0.5f;

        CollisionManager::AABB myAABB;
        myAABB.min = { pos.x - hw, pos.y - hh };
        myAABB.max = { pos.x + hw, pos.y + hh };

        // 作成した AABB を CollisionManager に登録
        m_pCollision->AddStaticAABB(myAABB);
    }
}
#include "NeedleFloor.h"

void NeedleFloor::Init(const char* texture, float x, float y, float width, float height)
{
    Enemy::Init(texture, x, y, width, height);

    // 【事前ロード】飾り用の画像を "Decoration" という名前のアニメーションとして登録
    // これで画像がメモリに乗り、後で PlayAnimation("Decoration") するだけで切り替わります
    m_object.AddAnimation("Decoration", "asset/Field/Needle_Floor_Decorated.png", 1, 1, 0, 0, 0, 1.0f, false, false, 999);
}

void NeedleFloor::Update(float deltaTime) 
{
    // ハリ床は移動しないので座標更新のみ
    m_object.Update(deltaTime);
}

void NeedleFloor::OnDecorated(bool justDeco) 
{

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

void NeedleFloor::Disable(bool justdeco)
{

    //Enemy::Disable(justdeco);

    if (m_state == State::Decorated)
    {
        return;
    }

    m_state = State::Decorated;

    m_object.PlayAnimation("Decoration");

    // Decorated → Platform登録（ただし許可されている場合のみ）
    if (m_state == State::Decorated &&
        m_enablePlatformRegisterOnDecorated &&
        !m_isPlatformRegistered)
    {
        //m_mark.SetSize(200, 150, 0);

        if (m_pCollision)
        {
            m_pCollision->SetTag(&m_object, ColliderTag::Platform);
            m_pCollision->AddStatic(&m_object);
            m_isPlatformRegistered = true;

            auto currentSize = m_object.GetSize();
            m_object.SetSize(currentSize.x, currentSize.y, currentSize.z);


            if (justdeco)
            {
                m_object.PlayAnimation("Decoration");
            }
            else
            {
                m_object.SetColor(1.0, 1.0, 1.0, 1.0);
            }
        }
    }

   /* if (justdeco)
    {
        m_object.PlayAnimation("Decoration");

        OnDecorated();
    }*/
}
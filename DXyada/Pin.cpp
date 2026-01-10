#include "Pin.h"

void Pin::Init()
{

}

void Pin::Init(const char* texture, float x, float y, float width, float height) 
{
    m_object.Init();
    m_object.AddTexture(texture);
    m_object.SetPos(x, y, 0);
    m_object.SetSize(width, height, 0);
    m_state = State::Normal;
}

void Pin::Update(float dt) 
{
    m_object.Update(dt);

    if (GetState() == State::Decorated)
    {
        m_object.SetColor(1.0, 0, 0, 0);
    }
}

void Pin::Draw() 
{
    m_object.Draw
    (
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
}

void Pin::OnHit() 
{
    if (m_state == State::Decorated)
    {
        return;
    }

    // 見た目を変える（リボン付き画像にするなど）
    // m_object.SetTexture("asset/pin_ribbon.png");

    // 足場として登録する
    if (m_pCollision) 
    {
        auto pos = m_object.GetPos();
        auto size = m_object.GetSize();

        CollisionManager::AABB aabb;
        aabb.min = { pos.x - size.x / 2, pos.y - size.y / 2 };
        aabb.max = { pos.x + size.x / 2, pos.y + size.y / 2 };

        m_pCollision->AddStaticAABB(aabb);
    }
}

void Pin::UnInit()
{
    m_object.UnInit();  
}

void Pin::SetPos(float Pos_X, float Pos_Y) {
    m_object.SetPos(Pos_X, Pos_Y, 0);
}

void Pin::SetState(State state)
{
    if (m_state == state) return;

    m_state = state;

    //Decoratedになったら自動的にPlatformとして登録
    if (m_state == State::Decorated && !m_isPlatformRegistered)
    {
        if (m_pCollision)
        {
            //Platformとして静的コリジョンに登録
            
            m_pCollision->SetTag(&m_object, ColliderTag::Platform);
            m_pCollision->AddStatic(&m_object);
            m_isPlatformRegistered = true;

            auto currentSize = m_object.GetSize();
            m_object.SetSize(currentSize.x * 4.0f, currentSize.y, currentSize.z);

            // テクスチャ変更
           m_object.AddTexture("asset/Field/Rippa.png");
        }
    }
}
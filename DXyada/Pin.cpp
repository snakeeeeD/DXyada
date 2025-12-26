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

    m_state = State::Decorated;

    // Œ©‚½–Ú‚ð•Ï‚¦‚éiƒŠƒ{ƒ“•t‚«‰æ‘œ‚É‚·‚é‚È‚Çj
    // m_object.SetTexture("asset/pin_ribbon.png");

    // ‘«ê‚Æ‚µ‚Ä“o˜^‚·‚é
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
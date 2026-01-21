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
        m_object.SetColor(1.0, 1.0, 1.0, 1.0);
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
    m_object.SetColor(1, 0, 0, 1); // ‚Ü‚¸Ô‚­‚µ‚ÄŠm”F
    // Œ©‚½–Ú‚ð•Ï‚¦‚éiƒŠƒ{ƒ“•t‚«‰æ‘œ‚É‚·‚é‚È‚Çj
     m_object.AddTexture("asset/block.png");

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

//void Pin::SetState(State state)
//{
//    if (m_state == state) return;
//
//    m_state = state;
//
//    //Decorated‚É‚È‚Á‚½‚çŽ©“®“I‚ÉPlatform‚Æ‚µ‚Ä“o˜^
//    if (m_state == State::Decorated && !m_isPlatformRegistered)
//    {
//        if (m_pCollision)
//        {
//            //Platform‚Æ‚µ‚ÄÃ“IƒRƒŠƒWƒ‡ƒ“‚É“o˜^
//            
//            m_pCollision->SetTag(&m_object, ColliderTag::Platform);
//            m_pCollision->AddStatic(&m_object);
//            m_isPlatformRegistered = true;
//
//            auto currentSize = m_object.GetSize();
//            m_object.SetSize(currentSize.x * 4.0f, currentSize.y, currentSize.z);
//
//            // ƒeƒNƒXƒ`ƒƒ•ÏX
//           m_object.AddTexture("asset/Field/block.png");
//        }
//    }
//}

void Pin::SetState(State state)
{
    if (m_state == state) return;

    m_state = state;

    // Decorated ¨ Platform“o˜^i‚½‚¾‚µ‹–‰Â‚³‚ê‚Ä‚¢‚éê‡‚Ì‚Ýj
    if (m_state == State::Decorated &&
        m_enablePlatformRegisterOnDecorated &&
        !m_isPlatformRegistered)
    {
        if (m_pCollision)
        {
            m_pCollision->SetTag(&m_object, ColliderTag::Platform);
            m_pCollision->AddStatic(&m_object);
            m_isPlatformRegistered = true;
            m_canBeHookedByRibbon = false;

            auto currentSize = m_object.GetSize();
            m_object.SetSize(currentSize.x * 4.0f, currentSize.y, currentSize.z);

            m_object.AddTexture("asset/Field/block.png");
        }
    }
}

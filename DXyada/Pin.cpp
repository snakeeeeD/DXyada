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

    auto pos = m_object.GetPos();
    auto sz = m_object.GetSize();

    struct MarkPosition {
        float offsetX;
        float offsetY;
        float angle;
    };

    MarkPosition positions[6] = {
        // グループ1 (m_mark用の2つ)
        { -100.0f, -50.0f, -15.0f },   // 位置1
        { -70.0f, -60.0f, 10.0f },    // 位置2

        // グループ2 (m_mark2用の2つ)
        { -20.0f, 55.0f, -5.0f },      // 位置3
        { 10.0f, 65.0f, 20.0f },      // 位置4

        // グループ3 (m_mark3用の2つ)
        { 70.0f, -50.0f, -20.0f },    // 位置5
        { 100.0f, -60.0f, 15.0f }      // 位置6
    };

    int mark1Index = rand() % 2;  // 0 または 1
    m_mark.Init();
    m_mark.AddTexture("asset/Field/星.png");
    m_mark.SetSize(0, 0, 0);
    m_mark.SetAngle(positions[mark1Index].angle);
    m_mark.SetPos(
        pos.x + positions[mark1Index].offsetX,
        pos.y + (sz.y * 0.5f) + positions[mark1Index].offsetY,
        0
    );

    int mark2Index = 2 + (rand() % 2);  // 2 または 3
    m_mark2.Init();
    m_mark2.AddTexture("asset/Field/星.png");
    m_mark2.SetSize(0, 0, 0);
    m_mark2.SetAngle(positions[mark2Index].angle);
    m_mark2.SetPos(
        pos.x + positions[mark2Index].offsetX,
        pos.y + (sz.y * 0.5f) + positions[mark2Index].offsetY,
        0
    );

    int mark3Index = 4 + (rand() % 2);  // 4 または 5
    m_mark3.Init();
    m_mark3.AddTexture("asset/Field/星.png");
    m_mark3.SetSize(0, 0, 0);
    m_mark3.SetAngle(positions[mark3Index].angle);
    m_mark3.SetPos(
        pos.x + positions[mark3Index].offsetX,
        pos.y + (sz.y * 0.5f) + positions[mark3Index].offsetY,
        0
    );

    m_object.AddAnimation("Decoration", "asset/Field/Pin_Decorated.png", 1, 1, 0, 0, 0, 1.0f, false, false, 999);
}

void Pin::Update(float dt)
{
    m_object.Update(dt);
    if (m_justDeco)
    {
        if (m_markVisible)
        {
            auto pos = m_object.GetPos();
            auto sz = m_object.GetSize();
           /* m_mark.SetPos(pos.x + m_markOffsetX, pos.y + (sz.y) + m_markOffsetY, 0);
            m_mark.Update(dt);*/

            m_mark.Update(dt);
            m_mark2.Update(dt);
            m_mark3.Update(dt);
        }
    }

    if (GetState() == State::Decorated)
    {
        m_object.SetColor(1.0, 1.0, 1.0, 1.0);
    }
}

void Pin::Draw()
{
    if (m_markVisible)
    {
       // m_mark.Draw(g_pDeviceContext, g_pInputLayout, g_pVertexShader, g_pPixelShader, g_pConstantBuffer);
        m_mark.Draw(g_pDeviceContext, g_pInputLayout, g_pVertexShader, g_pPixelShader, g_pConstantBuffer);
        m_mark2.Draw(g_pDeviceContext, g_pInputLayout, g_pVertexShader, g_pPixelShader, g_pConstantBuffer);
        m_mark3.Draw(g_pDeviceContext, g_pInputLayout, g_pVertexShader, g_pPixelShader, g_pConstantBuffer);
    }

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
    m_object.SetColor(1, 0, 0, 1); // まず赤くして確認
    // 見た目を変える（リボン付き画像にするなど）
    m_object.AddTexture("asset/floar.png");

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
    m_mark.UnInit();
    m_mark2.UnInit();
    m_mark3.UnInit();
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
//    //Decoratedになったら自動的にPlatformとして登録
//    if (m_state == State::Decorated && !m_isPlatformRegistered)
//    {
//        if (m_pCollision)
//        {
//            //Platformとして静的コリジョンに登録
//            
//            m_pCollision->SetTag(&m_object, ColliderTag::Platform);
//            m_pCollision->AddStatic(&m_object);
//            m_isPlatformRegistered = true;
//
//            auto currentSize = m_object.GetSize();
//            m_object.SetSize(currentSize.x * 4.0f, currentSize.y, currentSize.z);
//
//            // テクスチャ変更
//           m_object.AddTexture("asset/Field/block.png");
//        }
//    }
//}

void Pin::SetState(State state, bool justdeco)
{
    if (m_state == state) return;

    m_state = state;

    m_justDeco = justdeco;

    // Decorated → Platform登録（ただし許可されている場合のみ）
    if (m_state == State::Decorated &&
        m_enablePlatformRegisterOnDecorated &&
        !m_isPlatformRegistered)
    {
        m_mark. SetSize(50, 37.5, 0);
        m_mark2.SetSize(50, 37.5, 0);
        m_mark3.SetSize(50, 37.5, 0);

        if (m_pCollision)
        {
            m_pCollision->SetTag(&m_object, ColliderTag::Platform);
            m_pCollision->AddStatic(&m_object);
            m_isPlatformRegistered = true;

            auto currentSize = m_object.GetSize();
            m_object.SetSize(currentSize.x * 5.0f, currentSize.y, currentSize.z);

           // m_object.AddTexture("asset/Field/block.png");
            m_object.PlayAnimation("Decoration");

            if (justdeco)
            {
                //m_object.PlayAnimation("Decoration");
                m_markVisible = true;
                m_mark.SetColor(1, 1, 1, 1);
                m_mark2.SetColor(1, 1, 1, 1);
                m_mark3.SetColor(1, 1, 1, 1);
            }
            else
            {
                //m_object.SetColor(1.0, 1.0, 1.0, 1.0); 
                m_markVisible = false;
                m_mark.SetColor(1, 1, 1, 0);
                m_mark2.SetColor(1, 1, 1, 0);
                m_mark3.SetColor(1, 1, 1, 0);
            }
        }
    }
}

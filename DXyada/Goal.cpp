#include "Goal.h"
#include <cmath>

Goal::Goal()
    : m_triggerRange(400.0f)
    , m_wasPlayerNear(false)
{
}

Goal::~Goal()
{
    UnInit();
}

void Goal::Init(
    const std::string& signTexturePath,
    float posX,
    float posY,
    float width,
    float height
)
{
    m_object.Init();
    m_object.AddTexture(signTexturePath.c_str());
    m_object.SetPos(posX, posY, 0);
    m_object.SetSize(width, height, 0);
    m_object.SetColor(0.5f, 0.5f, 0.5f, 1.0f);
    m_wasPlayerNear = false;
}

void Goal::Update(float deltaTime, const DirectX::XMFLOAT3& playerPos)
{
    // デコレーションされてなければ終了
    if (!m_isDecorated)
        return;

    auto goalPos = m_object.GetPos();
    auto goalSize = m_object.GetSize();

    float left = goalPos.x - goalSize.x * 0.5f;
    float right = goalPos.x + goalSize.x * 0.5f;
    float top = goalPos.y + goalSize.y * 0.5f;
    float bottom = goalPos.y - goalSize.y * 0.5f;


    OutputDebugStringA("Goal: reached!\n");
    m_isReached = true;


    m_object.Update(deltaTime);
}



void Goal::Draw()
{
    m_object.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );

}

void Goal::UnInit()
{
    m_object.UnInit();
}
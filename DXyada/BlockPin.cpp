#include "BlockPin.h"
#include <cmath>

BlockPin::BlockPin()
    : m_moveSpeed(0.0f)
    , m_attachedPlatform(nullptr)
{
    m_prevPos = { 0,0,0 };
}

BlockPin::~BlockPin() {}

void BlockPin::Init(const char* texture, float x, float y, float width, float height)
{
    Pin::Init(texture, x, y, width, height);

    // îOÇÃÇΩÇﬂ Pin::Init Ç™ m_object ÇêGÇÈê›åvÇ≈Ç‡îjí]ÇµÇ»Ç¢ÇÊÇ§Ç…ñæé¶
    m_object.SetPos(x, y, 0.0f);
    m_object.SetSize(width, height, 0);

    m_prevPos = m_object.GetPos();
}

void BlockPin::Update(float dt)
{
    Pin::Update(dt);
}

void BlockPin::Draw()
{
    Pin::Draw();
}

void BlockPin::AttachPlatform(Platform* plat)
{
    m_attachedPlatform = plat;
    m_prevPos = m_object.GetPos();
}

void BlockPin::DetachPlatform()
{
    m_attachedPlatform = nullptr;
}

void BlockPin::OnWindUp(const DirectX::XMFLOAT3& playerPos, float dt, float playerSpeed)
{
    DirectX::XMFLOAT3 currentPos = m_object.GetPos();

    float dx = playerPos.x - currentPos.x;
    float dy = playerPos.y - currentPos.y;

    // Åöé≤êßå¿ÅFâ°ÇÃÇ› / ècÇÃÇ›
    if (m_moveAxis == MoveAxis::Horizontal) dy = 0.0f;
    if (m_moveAxis == MoveAxis::Vertical)   dx = 0.0f;

    float distance = std::sqrt(dx * dx + dy * dy);

    const float stopDistance = 80.0f;
    if (distance <= stopDistance || distance <= 0.0001f)
        return;

    float dirX = dx / distance;
    float dirY = dy / distance;

    m_moveSpeed = playerSpeed; // ä˘ë∂ì•èP

    currentPos.x += dirX * m_moveSpeed * dt;
    currentPos.y += dirY * m_moveSpeed * dt;

    // çsÇ´âﬂÇ¨ñhé~Åié≤êßå¿å„ÇÃ dx/dy ÇégÇ¡ÇƒîªíËÅj
    float newDx = playerPos.x - currentPos.x;
    float newDy = playerPos.y - currentPos.y;

    if ((dx * newDx < 0) || (dy * newDy < 0))
    {
        // é≤êßå¿Ç…çáÇÌÇπÇƒå≈íË
        if (m_moveAxis == MoveAxis::Horizontal)
            currentPos.x = playerPos.x;
        else if (m_moveAxis == MoveAxis::Vertical)
            currentPos.y = playerPos.y;
        else
        {
            currentPos.x = playerPos.x;
            currentPos.y = playerPos.y;
        }
    }

    m_object.SetPos(currentPos.x, currentPos.y, currentPos.z);
}
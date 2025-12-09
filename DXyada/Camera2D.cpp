#include "Camera2D.h"

DirectX::XMFLOAT3 g_cameraPos = { 0,0,0 };

void Camera::Init() {
    m_pos = { 0,0,0 };
}

void Camera::Update(const DirectX::XMFLOAT3& targetPos)
{
    // í«îˆÇ∑ÇÈÇæÇØÇÃä»à’ÉJÉÅÉâ

    m_offset.x = 500.0;
    m_offset.y = 300.0;

    m_pos.x = targetPos.x + m_offset.x;
    m_pos.y = targetPos.y + m_offset.y;
    m_pos.z = targetPos.z;

    m_pos.x += (targetPos.x + m_offset.x - m_pos.x) * 0.1f;
    m_pos.y += (targetPos.y + m_offset.y - m_pos.y) * 0.1f;

    g_cameraPos = m_pos;

}
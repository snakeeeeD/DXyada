#pragma once
#include <DirectXMath.h>

extern DirectX::XMFLOAT3 g_cameraPos;


class Camera {
private:
    DirectX::XMFLOAT3 m_pos;
    DirectX::XMFLOAT3 m_offset;
    float m_zoom;
public:
    void Init();
    void Update(const DirectX::XMFLOAT3& targetPos); // ’Ç”öƒJƒƒ‰‚Ìê‡
    DirectX::XMFLOAT3 GetPos() const { return g_cameraPos; }
    void Reset();
};
#pragma once
#include "Pin.h"
#include "Platform.h"
#include <DirectXMath.h>

class BlockPin : public Pin
{
public:
    enum class MoveAxis
    {
        Both,
        Horizontal,
        Vertical
    };
public:
    BlockPin();
    virtual ~BlockPin();

    void Init(const char* texture, float x, float y, float width, float height);
    void Update(float dt);
    void Draw();

    // 巻き取り：ピン自身が動く
    virtual void OnWindUp(const DirectX::XMFLOAT3& playerPos, float dt, float playerSpeed);
    
    // 巻き取りの移動の範囲設定
    // 右端、左端、上端、下端
    void SetLimitPos(float right, float left, float top, float down);

    void AttachPlatform(Platform* plat);
    void DetachPlatform();
    Platform* GetAttachedPlatform() const { return m_attachedPlatform; }

    void SetMoveAxis(MoveAxis axis) { m_moveAxis = axis; }
    MoveAxis GetMoveAxis() const { return m_moveAxis; }

private:
    float m_moveSpeed;

    float m_LimitRight=0;
    float m_LimitLeft=0;
    float m_LimitTop=0;
    float m_LimitDown=0;

    Platform* m_attachedPlatform;

    DirectX::XMFLOAT3 m_prevPos;

    MoveAxis m_moveAxis = MoveAxis::Both;

};

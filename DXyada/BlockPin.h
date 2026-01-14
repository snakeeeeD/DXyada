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

    // ä™Ç´éÊÇËÅFÉsÉìé©êgÇ™ìÆÇ≠
    virtual void OnWindUp(const DirectX::XMFLOAT3& playerPos, float dt, float playerSpeed);

    void AttachPlatform(Platform* plat);
    void DetachPlatform();
    Platform* GetAttachedPlatform() const { return m_attachedPlatform; }

    void SetMoveAxis(MoveAxis axis) { m_moveAxis = axis; }
    MoveAxis GetMoveAxis() const { return m_moveAxis; }

private:
    float m_moveSpeed;

    Platform* m_attachedPlatform;

    DirectX::XMFLOAT3 m_prevPos;

    MoveAxis m_moveAxis = MoveAxis::Both;

};

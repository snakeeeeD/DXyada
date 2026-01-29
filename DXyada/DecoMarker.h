#pragma once
#include "Object.h"
#include <DirectXMath.h>

class DecoMarker
{
public:
    void Init(const char* tex);
    void Hide();

    // target の頭上に出す（オフセットとサイズ固定）
    void ShowAtHead(Object* target, float offsetY = 120.0f, float w = 300.0f, float h = 200.0f);

    void Update(float dt, Object* target = nullptr);
    Object* GetObject() { return &m_obj; }

    Object* GetObject2() { return &m_obj2; }
    Object* GetObject3() { return &m_obj3; }
    void SetTexture(const char* tex)
    {
        m_obj.AddTexture(tex); // あなたのObject仕様が「差し替え」ならOK
    }

    void SetMoveMode(bool isMove) { m_isMove = isMove; }
    bool GetisMove() const { return m_isMove; }

private:
    Object m_obj;
    Object m_obj2;
    Object m_obj3;
    bool   m_visible = true;

    float m_offsetX1 = 0.0f;
    float m_offsetY1 = 0.0f;
    float m_offsetX2 = 0.0f;
    float m_offsetY2 = 0.0f;
    float m_offsetX3 = 0.0f;
    float m_offsetY3 = 0.0f;

    bool m_isMove = false;
};

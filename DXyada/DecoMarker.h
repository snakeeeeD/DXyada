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

    void Update(float dt);
    Object* GetObject() { return &m_obj; }
    void SetTexture(const char* tex)
    {
        m_obj.AddTexture(tex); // あなたのObject仕様が「差し替え」ならOK
    }

private:
    Object m_obj;
    bool m_visible = false;

};

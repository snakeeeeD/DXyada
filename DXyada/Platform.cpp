#include "Platform.h"

// デフォルト Init
void Platform::Init() {
}

// 引数付き Init
void Platform::Init(const char* texture, float x, float y, float width, float height) {
    m_object.Init(texture);
    m_object.SetPos(x, y, 0);
    m_object.SetSize(width, height, 0);
}

void Platform::Update() {
    m_object.Update(1/240.0f);
}

void Platform::Draw() {
    m_object.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
}

void Platform::UnInit() {
    m_object.UnInit();
}

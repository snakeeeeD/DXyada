#include "Hariyama.h"

void Hariyama::Init(const char* texture, float x, float y)
{
    m_object.Init();
    //m_object.AddAnimation("", 1, 0, 0, 64, 64, 1.0f); // 画像のサイズに合わせて調整
    m_object.SetPos(x, y, 0);
    //m_object.SetSize(64, 64); // サイズは画像の大きさに合わせて調整してください
    m_direction = -1.0f;      // 最初は左向き
}

void Hariyama::Update(float dt)
{
    // 今は動かさなくて良いので、Updateは空か、反転のテスト用にフラグセットだけ
    m_object.SetFlipX(m_direction > 0);
    m_object.Update(dt);
}

void Hariyama::Draw()
{
    // PlayerやRippaと同じ描画命令
    m_object.Draw
    (
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
}
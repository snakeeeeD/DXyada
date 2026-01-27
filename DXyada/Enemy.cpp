#include "Enemy.h"
#include<DirectXMath.h>
#include "input.h"
#include"SceneManager.h"

extern Input input;
// デフォルト Init
void Enemy::Init()
{

}

// 引数付き Init
void Enemy::Init(const char* texture, float x, float y, float width, float height)
{
    m_object.Init();
    m_object.AddTexture(texture);
    m_object.SetPos(x, y, 0);
    m_object.SetSize(width, height, 0);

    m_mark.Init();
    m_mark.AddTexture("asset/Field/back.png");
    m_mark.SetSize(0, 0, 0);
    int angle = (rand() % 61) - 30;
    m_mark.SetAngle(angle);

    auto pos = m_object.GetPos();
    auto sz = m_object.GetSize();
    m_mark.SetPos(pos.x + m_markOffsetX, pos.y + (sz.y * 0.5f) + m_markOffsetY, 0);


    deltaTime = 1.0f / 240.0f;

    m_object.AddAnimation("Normal", texture,
        1, 1, 0, 0, 0, 0.0f, false, false, 0);

    if (!m_decoratedTexturePath.empty() && !m_hasChangedTexture)
    {
        m_object.AddAnimation("Deco", m_decoratedTexturePath.c_str(),
            1, 1, 0, 0, 0, 0.0f, false, false, 0);

        m_hasChangedTexture = true;
    }
}

void Enemy::Disable(bool justdeco)
{
    if (!m_enableCollision)
        return;

    m_enableCollision = false;
    m_enemystate = EnemyState::YouDied;

    if (!m_decoratedTexturePath.empty() && !m_hasChangedTexture)
    {
        m_object.PlayAnimation("Deco");

        m_hasChangedTexture = true;
    }

    if (justdeco)
    {
        //m_object.SetColor(1, 0, 0, 0.3f);
    }
    else
    {
        // 半透明にする
        m_object.SetColor(1, 1, 1, 0.3f);
    }

}


void Enemy::Update(float deltaTime)
{
    if (m_enemystate == EnemyState::YouDied)
        return;

    auto pos = m_object.GetPos();

    m_object.SetPos(pos.x, pos.y, pos.z);

    // Objectのアニメーション更新
    m_object.Update(1 / 240.0f);

    if (m_markVisible)
    {
        auto pos = m_object.GetPos();
        auto sz = m_object.GetSize();
        m_mark.SetPos(pos.x + m_markOffsetX, pos.y + (sz.y * 0.5f) + m_markOffsetY, 0);
        m_mark.Update(deltaTime);
    }
}

void Enemy::Draw()
{

    OutputDebugStringA("Enemy Draw\n");

    if (m_markVisible)
    {
        m_mark.Draw(g_pDeviceContext, g_pInputLayout, g_pVertexShader, g_pPixelShader, g_pConstantBuffer);
    }

    m_object.Draw
    (
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
}

void Enemy::UnInit()
{
    m_object.UnInit();
    m_mark.UnInit();
}

void Enemy::OnDecorated()
{
    m_isDecorated = true;

    if (!m_decoratedTexturePath.empty() && !m_hasChangedTexture)
    {
        m_object.PlayAnimation("Deco");
        m_hasChangedTexture = true;
    }
}
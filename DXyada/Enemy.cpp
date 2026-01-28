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

    auto pos = m_object.GetPos();
    auto sz = m_object.GetSize();

    struct MarkPosition {
        float offsetX;
        float offsetY;
        float angle;
    };

    MarkPosition positions[6] = {
        // グループ1 (m_mark用の2つ)
        { -100.0f, -50.0f, -15.0f },   // 位置1
        { -70.0f, -60.0f, 10.0f },    // 位置2

        // グループ2 (m_mark2用の2つ)
        { -20.0f, 55.0f, -5.0f },      // 位置3
        { 10.0f, 65.0f, 20.0f },      // 位置4

        // グループ3 (m_mark3用の2つ)
        { 70.0f, -50.0f, -20.0f },    // 位置5
        { 100.0f, -60.0f, 15.0f }      // 位置6
    };

    int mark1Index = rand() % 2;  // 0 または 1
    m_mark.Init();
    m_mark.AddTexture("asset/Field/星.png");
    m_mark.SetSize(0, 0, 0);
    m_mark.SetAngle(positions[mark1Index].angle);
    m_mark.SetPos(
        pos.x + positions[mark1Index].offsetX,
        pos.y + (sz.y * 0.5f) + positions[mark1Index].offsetY,
        0
    );

    int mark2Index = 2 + (rand() % 2);  // 2 または 3
    m_mark2.Init();
    m_mark2.AddTexture("asset/Field/星.png");
    m_mark2.SetSize(0, 0, 0);
    m_mark2.SetAngle(positions[mark2Index].angle);
    m_mark2.SetPos(
        pos.x + positions[mark2Index].offsetX,
        pos.y + (sz.y * 0.5f) + positions[mark2Index].offsetY,
        0
    );

    int mark3Index = 4 + (rand() % 2);  // 4 または 5
    m_mark3.Init();
    m_mark3.AddTexture("asset/Field/星.png");
    m_mark3.SetSize(0, 0, 0);
    m_mark3.SetAngle(positions[mark3Index].angle);
    m_mark3.SetPos(
        pos.x + positions[mark3Index].offsetX,
        pos.y + (sz.y * 0.5f) + positions[mark3Index].offsetY,
        0
    );

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

    m_mark.SetSize(50, 37.5, 0);
    m_mark2.SetSize(50, 37.5, 0);
    m_mark3.SetSize(50, 37.5, 0);

    if (justdeco)
    {
        m_mark.SetColor(1, 1, 1, 1);
        m_mark2.SetColor(1, 1, 1, 1);
        m_mark3.SetColor(1, 1, 1, 1);
    }
    else
    {
        m_mark.SetColor(1, 1, 1, 0);
        m_mark2.SetColor(1, 1, 1, 0);
        m_mark3.SetColor(1, 1, 1, 0);
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
        m_mark.Update(deltaTime);
        m_mark2.Update(deltaTime);
        m_mark3.Update(deltaTime);
    }
}

void Enemy::Draw()
{

    OutputDebugStringA("Enemy Draw\n");

    if (m_markVisible)
    {
        m_mark.Draw(g_pDeviceContext, g_pInputLayout, g_pVertexShader, g_pPixelShader, g_pConstantBuffer);
        m_mark2.Draw(g_pDeviceContext, g_pInputLayout, g_pVertexShader, g_pPixelShader, g_pConstantBuffer);
        m_mark3.Draw(g_pDeviceContext, g_pInputLayout, g_pVertexShader, g_pPixelShader, g_pConstantBuffer);
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
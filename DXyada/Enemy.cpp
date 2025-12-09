#include "Enemy.h"
#include<DirectXMath.h>
#include "input.h"
#include"SceneManager.h"

extern Input input;
// デフォルト Init
void Enemy::Init() {
}

// 引数付き Init
void Enemy::Init(const char* texture, float x, float y, float width, float height) {
    m_object.Init(texture);
    m_object.SetPos(x, y, 0);
    m_object.SetSize(width, height, 0);

    deltaTime = 1.0f / 240.0f;

}

void Enemy::Update(float deltaTime) {
    auto pos = m_object.GetPos();

    //if (move==false) {
    //    pos.x += 1;
    //}
    //if (pos.x >= 600) {
    //    move = true;
    //}
    //if (move == true) {
    //    pos.x -= 1;
    //}
    //if (pos.x <= 300) {
    //    move = false;
    //}

  
    m_object.SetPos(pos.x, pos.y, pos.z);

    // Objectのアニメーション更新
    m_object.Update(1 / 240.0f);
}

void Enemy::Draw() {
    m_object.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
}

void Enemy::UnInit() {
    m_object.UnInit();
}

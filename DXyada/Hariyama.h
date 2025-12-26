#pragma once
#include "Enemy.h"

class Hariyama : public Enemy
{
public:
    Hariyama() = default;
    ~Hariyama() = default;

    // 初期化、更新、描画（リッパーと同じ構成）
    void Init(const char* filename, float x, float y);
    void Update(float dt) override;
    void Draw() override;

private:
    // アニメーション動作のみを管理する変数
    int animCount = 0;
};
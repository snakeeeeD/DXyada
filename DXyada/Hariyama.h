#pragma once
#include "Enemy.h"

class Hariyama : public Enemy
{
public:
    Hariyama() = default;
    ~Hariyama() = default;

    void Update(float dt) override;

    void SetEnemyList(std::vector<Enemy*>* list) { m_pEnemyList = list; }

private:

    float m_shotTimer = 0.0f; // 弾発射タイマー
    const float SHOT_INTERVAL = 3.0f; // 弾発射間隔

    //画面サイズ
    const float ACTIVATION_RANGE = 1.5f * 1920.0f;

    void Shoot(); // 弾発射処理

    std::vector<Enemy*>* m_pEnemyList = nullptr; // 弾を追加するための敵リストへのポインタ

    // アニメーション動作のみを管理する変数
    int animCount = 0;
};

class HariyamaNeedle : public Enemy
{
    public:
    HariyamaNeedle(DirectX::XMFLOAT3 targetPos, DirectX::XMFLOAT3 selfPos);
	~HariyamaNeedle() = default;

	void Update(float dt) override;

	private:
	float m_speed = 180.0f; // 移動速度
    DirectX::XMFLOAT2 m_moveVec; //飛ばす方向
    float m_lifeTimer = 0.0f; //消滅までのタイマー
};
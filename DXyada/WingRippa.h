#pragma once
#include "Enemy.h"
#include "Texture.h"

class WingRippa : public Enemy
{
public:
	enum class Type
	{
		ZigZag,
		Wandering_Circle
	};

private:
	Type m_type = Type::ZigZag;

	// --- 移動パラメータ ---
	float m_walkspeed = 120.0f;    // X及びYの移動速度
	float m_amplitude = 60.0f;     // 上下の振れ幅
	float m_frequency = 1.5f;      // 上下の揺れスピード
	float m_radius = 100.0f;       // 円の半径

	// --- 制御用変数 ---
	float m_time = 0.0f;           // 経過時間（サイン・コサイン計算用）
	float m_baseX = 0.0f;          // 出現時のX（円の中心）
	float m_baseY = 0.0f;          // 出現時のY（揺れの基準）
	int m_direction = -1;          // -1 : 左, 1 : 右
	int m_verticalDir = 1;		   // -1 : 下, 1 : 上 

	int m_zigzagCount = 0;		   // ジグザグ回数カウント
	int m_maxZigzag = 5;		   // ジグザグ最大回数

	bool m_InitBase = false; // 初期位置設定フラグ

public:
	WingRippa(Type type = Type::ZigZag);

	void SetType(Type type) { m_type = type; }

	void Update(float deltaTime) override;

	void UpdateZigZag(float dt); //Normal用
	void UpdateCircle(float dt); // Wandering用
	void CheckWallCollision(); // 壁衝突チェック

};
#pragma once
#include"Object.h"
#include <vector>   
#include"Platform.h"
#include"Enemy.h"
#include"Ribbon.h"

extern DirectX::XMFLOAT3 g_StartPlayer;


class Player {
private:
	Object m_player;
	Object m_guideline;
	Ribbon m_ribbon;

	float deltaTime;//一度読み込むたびに何秒分すすむか

	float m_velY = 0.0f;
	float m_gravity = 200.0f;
	bool m_isOnGround = false;
	float m_jumpPower = 600.0f;
	float m_width = 100.0f;
	float m_height = 150.0f;


	int m_inputDir = 0;	//プレイヤーの左右入力優先用　0：入力なし　1：左入力　-1：右入力

	//HP関連
	int m_hp;
	int m_maxhp;
	bool m_isInvincible;			//無敵時間中かどうか
	float m_Invincibletimer;		//無敵時間タイマー
	float m_InvincibleDuration;	//無敵時間の長さ（秒）

	//ノックバック関連
	bool m_isKnockback;
	float m_knockbackTimer;
	float m_knockbackDuration;
	DirectX::XMFLOAT2 m_knockbackVelocity;

	//実験用
	float t;
public:
	Tag tag = Tag::Player;

	Object* GetObject() { return &m_player; }
	Object* GetGuideLineObject() { return &m_guideline; }
	void Init();
	void SetPos(float Pos_X, float Pos_Y);
	void Update(float deltaTime, const std::vector<Platform>& platforms, const std::vector<Enemy>& Enemy);
	void Draw();
	void Uninit();

	void TakeDamage(int damage, DirectX::XMFLOAT2 knockbackDir);
	int GetHP() const { return m_hp; }
	int GetMaxHP() const { return m_maxhp; }
	bool isDead() const { return m_hp <= 0; }
	bool IsInvincible() const { return m_isInvincible; }

	// リボン取得用
	Ribbon& GetRibbon();

};


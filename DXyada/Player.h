#pragma once
#include"Object.h"
#include <vector>   
#include"Platform.h"
#include"Enemy.h"
#include"Ribbon.h"
#include "Pin.h"
#include "BlockPin.h"
#include "RemoteWindPin.h"

extern DirectX::XMFLOAT3 g_StartPlayer;

enum Player_AnimState
{
	Idle,
	Run,
	Jump,
	Damage,
	Throw,
	RibbonJump,
	Pulled,
	Roll
};

class CollisionManager;

class Player 
{
private:
	Object m_player;
	Object m_guideline;
	Ribbon m_ribbon;
	Object m_Circle;

	float deltaTime;//一度読み込むたびに何秒分すすむか

	float m_velY = 0.0f;
	float m_gravity = 200.0f;
	bool m_isOnGround = false;
	float m_jumpPower = 600.0f;
	float m_width = 100.0f;
	float m_height = 150.0f;

	bool m_moveLeft = false;
	bool m_moveRight = false;

	bool m_isCanMove = false;

	bool m_isLastRightDirection = true;	//最終右むいてるか

	bool m_isPinJumping = false;
	DirectX::XMFLOAT2 m_pinJumpVelocity = { 0.0f, 0.0f };

	int m_inputDir = 0;	//プレイヤーの左右入力優先用　0：入力なし　1：左入力　-1：右入力

	//HP関連
	int m_hp;
	int m_maxhp;
	bool m_isInvincible;			//無敵時間中かどうか
	float m_Invincibletimer;		//無敵時間タイマー
	float m_InvincibleDuration;		//無敵時間の長さ（秒）

	//ノックバック関連
	bool m_isKnockback;
	float m_knockbackTimer;
	float m_knockbackDuration;
	DirectX::XMFLOAT2 m_knockbackVelocity;
	bool m_Rightknockback = true;

	//リボン用
	bool m_isRibbonOut = false;		//リボンが出ているかどうか
	bool m_wasRTPressed = false;	//前フレームにRTが押されてるか

	//リボン用オートエイム
	DirectX::XMFLOAT2 m_ribbonAimDir{ 1.0f, 0.0f };
	float m_ribbonAimLength = 0.0f;
	Enemy* m_ribbonTargetEnemy = nullptr;

	float m_baseGuidelineLength;		//元の指示線の長さ
	bool m_hasTarget;					//ターゲットがあるか	
	DirectX::XMFLOAT3 m_targetPosition;	//ターゲットの座標用
	DirectX::XMFLOAT2 m_aimDirection;	//エイム方向を保存
	float m_aimAssistAngle = 20.0f;     // エイムアシストの角度範囲（度）

	//RT長押し撃破用
	float m_holdRTTimer;           //RT長押し時間
	float m_holdRTRequired;        //RT長押しの最大時間

	//LT長押し撃破用
	float m_holdLTTimer;           //LT長押し時間
	float m_holdLTRequired;        //撃破に必要な長押し時間
	Enemy* m_targetEnemy;          //撃破対象の敵

	//検出範囲の設定
	float m_detectionRangeSquare = m_height * 3.5;  //四角の範囲（半径）
	float m_detectionRangeCircle = m_height * 3.5;  //円の範囲（半径）

	//アニメーション関連
	enum class AnimationState
	{
		Normal = 0,		//通常（アイドル、移動）
		Action = 1,		//アクション（リボン）
		Damage = 2		//ダメージ、最優先
	};
	AnimationState m_currentAnimState = AnimationState::Normal;
	std::string m_lastPlayedAnim = "";	//前フレームのアニメーションの名前

	Player_AnimState m_animState = Idle;
	Player_AnimState m_prevAnimState = Idle;

	bool m_prevIsRightDirection;
	bool m_isThrowingThisFrame;

	bool m_isThrowAnimLock = false;
	bool m_throwDirectionRight = false;

	bool m_waitReleaseAfterThrow;

	// CollisionManager用
	CollisionManager* m_collisionMgr = nullptr;

	// 右スティック回転検出用
	float m_prevStickAngle = 0.0f;
	float m_totalRotation = 0.0f;
	bool m_isRotating = false;
	bool m_isPulling = false;
	bool m_isRolling = false;
	float m_pullSpeed = 0.0f;

	//実験用
	float t;
public:
	Tag tag = Tag::Player;

	Object* GetObject() { return &m_player; }
	Object* GetGuideLineObject() { return &m_guideline; }
	Object* GetCircle() { return &m_Circle; }
	void Init();
	void SetPos(float Pos_X, float Pos_Y);
	void Update(
		float deltaTime,
		std::vector<Platform*>& platforms,
		std::vector<Enemy*>& enemies,
		std::vector<Pin*>& pins
	); 
	void Draw();
	void Uninit();

	void TakeDamage(int damage, DirectX::XMFLOAT2 knockbackDir);
	int GetHP() const { return m_hp; }
	int GetMaxHP() const { return m_maxhp; }
	bool isDead() const { return m_hp <= 0; }
	bool IsInvincible() const { return m_isInvincible; }
	bool IsLastDirection() const { return m_isLastRightDirection; }	//最終向いてた方向を取得

	//範囲内の敵を取得
	bool IsEnemyInRange(const DirectX::XMFLOAT3& enemyPos, float& distance) const;

	void ApplyAnimation();

	// リボン取得用
	Ribbon& GetRibbon();

	// CollisionManager設定用
	void SetCollisionManager(CollisionManager* mgr);
};


#pragma once
#include"Object.h"
#include <vector>   
#include"Platform.h"
#include"Enemy.h"

extern DirectX::XMFLOAT3 g_StartPlayer;


class Player{
	private:
		Object m_player;
		Object m_guideline;
		float deltaTime;//一度読み込むたびに何秒分すすむか

		float m_velY = 0.0f;        
		float m_gravity = 2000.0f;  
		bool m_isOnGround = false;  
		float m_jumpPower = 600.0f; 
		float m_width = 100.0f;     
		float m_height = 150.0f;    

		int m_inputDir = 0;	//プレイヤーの左右入力優先用　0：入力なし　1：左入力　-1：右入力

		//実験用
		float t;
	public:
		Tag tag = Tag::Player;

		Object* GetObject() { return &m_player; }
		void Init();
		void SetPos(float Pos_X, float Pos_Y);
		void Update(float deltaTime, const std::vector<Platform>& platforms, const std::vector<Enemy>& Enemy);
		void Draw();
		void Uninit();
};


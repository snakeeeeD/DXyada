#pragma once
#include <vector>
#include <DirectXMath.h>
#include "CollisionManager.h"

class Object;
class Enemy;
class Pin;

//==============================
// ・リボンを複数のObjectで構成
//==============================
class Ribbon
{
public:
    //==============================
    // リボンの状態
    //==============================
    enum class State
    {
        Idle,       // 待機
        Throwing,   // 伸びる
        Returning   // 戻る
    };

private:
    //==============================
    // リボン1節分の情報
    //==============================
    struct Segment
    {
        Object* obj;                 // 描画用オブジェクト
        DirectX::XMFLOAT2 pos;       // 現在位置
    };

    //==============================
    // リボンの節配列
    //==============================
    std::vector<Segment> m_segments;

    //==============================
    // プレイヤー位置
    //==============================
    DirectX::XMFLOAT2 m_playerPos{};

    //==============================
    // 放物線用
    //==============================
    DirectX::XMFLOAT2 m_velocity{};
    float m_time = 0.0f;
    float m_gravity = 0.0f;
    float m_speed = 1400.0f;

    //==============================
    // 状態
    //==============================
    State m_state = State::Idle;

    //==============================
    // 設定値
    //==============================
    int   m_maxSegments = 20;
    float m_segmentLength = 30.0f;
    float m_maxLength = 0.0f;   // 伸びきる最大距離

    // 現在のリボン長さ
    float m_currentLength = 0.0f;

    // 投げる方向（正規化済み）
    DirectX::XMFLOAT2 m_direction{};

    //==============================
    // Enemy関連
    //==============================
    bool m_hasHit = false;
    Enemy* m_hitEnemy = nullptr;
    DirectX::XMFLOAT2 m_hitPos{};
    CollisionManager* m_collisionMgr = nullptr;



    bool m_isRTheld = false;

    //当たったピンを覚える
    Pin* m_hitPin = nullptr;

public:
    Ribbon();
    ~Ribbon();

    // 初期化（Objectの生成）
    void Init();

    // 更新（位置の計算）
    void Update(float deltaTime, const std::vector<Enemy*>& enemies, std::vector<Pin*>& pins);

    // プレイヤー位置セット
    void SetPlayerPos(const DirectX::XMFLOAT2& pos);

    // 現在のリボン状態を取得
    State GetState() const { return m_state; }

    // 投げる
    void Throw(
        const DirectX::XMFLOAT2& direction,
        float maxLength
    );


    //Enemy関連
    bool HasHit() const { return m_hasHit; }
    DirectX::XMFLOAT2 GetHitPos() const { return m_hitPos; }
    Enemy* GetHitEnemy() const { return m_hitEnemy; }
    Pin* GetHitPin() const { return m_hitPin; }
    void SetCollisionManager(CollisionManager* mgr)
    {
        m_collisionMgr = mgr;
    }

    // 戻す
    void Return();

    // リセット
    void Reset();

    // Stage側でDrawListに積む用
    std::vector<Object*> GetDrawObjects() const;

    void SetRTheld(bool held) { m_isRTheld = held; }

    // 解放
    void UnInit();
};

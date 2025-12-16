#pragma once
#include <vector>
#include <DirectXMath.h>

class Object;

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

public:
    Ribbon();
    ~Ribbon();

    // 初期化（Objectの生成）
    void Init();

    // 更新（位置の計算）
    void Update(float deltaTime);

    // プレイヤー位置セット
    void SetPlayerPos(const DirectX::XMFLOAT2& pos);

    // 現在のリボン状態を取得
    State GetState() const { return m_state; }

    // 投げる
    void Throw(const DirectX::XMFLOAT2& direction);

    // 戻す
    void Return();

    // リセット
    void Reset();

    // Stage側でDrawListに積む用
    std::vector<Object*> GetDrawObjects() const;

    // 解放
    void UnInit();
};

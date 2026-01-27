#pragma once
#include "Object.h"
#include "CollisionManager.h"
#include <string>

class Goal
{
private:
    Object m_object;                    // 看板のオブジェクト
    Object m_tutorialDisplay;           // チュートリアル表示用オブジェクト
    CollisionManager* m_pCollision = nullptr;

    // 判定用
    float m_triggerRange;               // プレイヤーが近づいたら反応する範囲
    bool m_wasPlayerNear;               // 前フレームでプレイヤーが近くにいたか
    bool m_isReached = false;
    bool m_isDecorated = false;
public:
    Goal();
    ~Goal();

    // 初期化
    void Init(
        const std::string& signTexturePath,
        float posX,
        float posY,
        float width,
        float height
    );

    // 更新
    void Update(float deltaTime, const DirectX::XMFLOAT3& playerPos);

    // 描画
    void Draw();

    // 解放
    void UnInit();

    // CollisionManager設定
    void SetCollisionManager(CollisionManager* collision) { m_pCollision = collision; }

    // ゲッター
    Object* GetObject() { return &m_object; }
    bool IsPlayerNear() const { return m_wasPlayerNear; }
    bool IsReached() const { return m_isReached; }
    void SetDecorated(bool decorated) { m_isDecorated = decorated; }
    bool IsDecorated() const { return m_isDecorated; }
    void SetPos(float x, float y)
    {
        m_object.SetPos(x, y, 0);
    }

};
#pragma once
#include "Object.h"
#include "CollisionManager.h"
#include <string>

class Tutorial
{
public:
    // チュートリアルの種類
    enum class Type
    {
        Move,           // 移動操作
        Jump,           // ジャンプ
        Aim,            // エイム
        RibbonBasic,    // リボン基本
        RibbonPull,     // リボン引き寄せ
        Decorate,       // 飾り付け
        Custom          // カスタム（独自の画像）
    };

private:
    Object m_object;                    // 看板のオブジェクト
    Object m_tutorialDisplay;           // チュートリアル表示用オブジェクト
    CollisionManager* m_pCollision = nullptr;

    // チュートリアル設定
    Type m_type;                        // チュートリアルの種類
    std::string m_tutorialImagePath;    // カスタムチュートリアル画像パス
    bool m_showTutorial;                // チュートリアルを表示するか
    bool m_isCheckpoint;                // チェックポイントとして機能するか

    // チェックポイント情報
    DirectX::XMFLOAT3 m_respawnPosition; // 復活地点
    bool m_isActivated;                  // このチェックポイントが有効化されているか

    // 判定用
    float m_triggerRange;               // プレイヤーが近づいたら反応する範囲
    bool m_wasPlayerNear;               // 前フレームでプレイヤーが近くにいたか

    // アニメーション用
    bool m_hasAnimation;                // アニメーションを使うか
    float m_displayAlpha;               // 表示の透明度（フェードイン/アウト用）
    float m_fadeSpeed;                  // フェード速度

public:
    Tutorial();
    ~Tutorial();

    // 初期化
    void Init(
        const std::string& signTexturePath,
        float posX,
        float posY,
        float width,
        float height,
        Type type = Type::Custom,
        bool showTutorial = true,
        bool isCheckpoint = true
    );

    // チュートリアル表示の初期化（画像の場合）
    void InitTutorialImage(
        const std::string& imagePath,
        float displayWidth,
        float displayHeight,
        float offsetX = 0.0f,   // 看板からの相対位置X
        float offsetY = 0.0f  // 看板からの相対位置Y
    );

    // チュートリアル表示の初期化（アニメーションの場合）
    void InitTutorialAnimation(
        const std::string& imagePath,
        int row,
        int col,
        int startRow,
        int startCol,
        int endRow,
        int endCol,
        bool loop,
        int animSpeed,
        float displayWidth,
        float displayHeight,
        float offsetX = 0.0f,
        float offsetY = 100.0f
    );

    // 更新
    void Update(float deltaTime, const DirectX::XMFLOAT3& playerPos);

    // 描画
    void Draw();

    // 解放
    void UnInit();

    // CollisionManager設定
    void SetCollisionManager(CollisionManager* collision) { m_pCollision = collision; }

    // チュートリアル設定
    void SetShowTutorial(bool show) { m_showTutorial = show; }
    void SetTriggerRange(float range) { m_triggerRange = range; }
    void SetFadeSpeed(float speed) { m_fadeSpeed = speed; }

    // チェックポイント設定
    void SetCheckpoint(bool enabled) { m_isCheckpoint = enabled; }
    void SetRespawnPosition(float x, float y, float z) { m_respawnPosition = { x, y, z }; }
    void Activate() { m_isActivated = true; }
    void Deactivate() { m_isActivated = false; }

    // ゲッター
    Object* GetObject() { return &m_object; }
    Object* GetDisplayObject() { return &m_tutorialDisplay; }
    bool IsShowingTutorial() const { return m_showTutorial; }
    bool IsCheckpoint() const { return m_isCheckpoint; }
    bool IsActivated() const { return m_isActivated; }
    DirectX::XMFLOAT3 GetRespawnPosition() const { return m_respawnPosition; }
    bool IsPlayerNear() const { return m_wasPlayerNear; }
    float GetDisplayAlpha() const { return m_displayAlpha; }
};
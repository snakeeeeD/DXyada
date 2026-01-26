//#pragma once
//class DecorMarkSystem
//{
//};
//#pragma once
//#include <vector>
//#include <unordered_map>
//#include <string>
//
//#include "Object.h"
//#include "CollisionManager.h"   // ColliderTag を使う想定
//
//// -------------------------
//// マーク種別
//// -------------------------
//enum class DecorMarkType
//{
//    None = 0,
//    Ribbon,     // Pin 等
//    Warning,    // Enemy 等
//    Anchor      // Platform 化 等
//};
//
//// -------------------------
//// Decorated 通知（イベント）を受ける側
//// -------------------------
//class IDecorEventListener
//{
//public:
//    virtual ~IDecorEventListener() {}
//    virtual void OnDecorated(Object* target, ColliderTag tag) = 0;
//    virtual void OnUndecorated(Object* target) = 0;
//
//    // 対象が消える（破棄される）直前に呼べるなら呼ぶ（推奨）
//    virtual void OnTargetDestroyed(Object* target) = 0;
//};
//
//// -------------------------
//// マークの実体
//// -------------------------
//struct DecorMarker
//{
//    Object        icon;     // 表示用
//    Object* target;   // 追従対象（※寿命管理は呼び出し側で守る）
//    DecorMarkType type;
//    ColliderTag   tag;
//
//    float         offsetX;
//    float         offsetY;
//    float         sizeW;
//    float         sizeH;
//
//    bool          active;
//
//    DecorMarker()
//        : target(nullptr)
//        , type(DecorMarkType::None)
//        , tag(ColliderTag::None)
//        , offsetX(0.0f)
//        , offsetY(0.0f)
//        , sizeW(32.0f)
//        , sizeH(32.0f)
//        , active(false)
//    {}
//};
//
//// -------------------------
//// マーク管理
//// -------------------------
//class DecorMarkSystem : public IDecorEventListener
//{
//public:
//    DecorMarkSystem();
//    ~DecorMarkSystem();
//
//    // 初期化：共通テクスチャなどをセット
//    void Init();
//
//    // 更新・描画
//    void Update(float dt);
//    void Draw();
//
//    // ステージ切替等で一括クリア
//    void Clear();
//
//    // 手動操作（必要なら）
//    void AddMarker(Object* target, ColliderTag tag);
//    void RemoveMarker(Object* target);
//
//    // ----- IDecorEventListener -----
//    virtual void OnDecorated(Object* target, ColliderTag tag) override;
//    virtual void OnUndecorated(Object* target) override;
//    virtual void OnTargetDestroyed(Object* target) override;
//
//    // 設定
//    void SetEnabled(bool enabled) { m_enabled = enabled; }
//    bool IsEnabled() const { return m_enabled; }
//
//    // テクスチャパスの差し替え（後から簡単に変更できる）
//    void SetTextureFor(DecorMarkType type, const std::string& path);
//
//private:
//    DecorMarkType DecideMarkType(ColliderTag tag) const;
//    const char* GetTexturePath(DecorMarkType type) const;
//
//    void ApplyIconVisual(DecorMarker& m);
//    void FollowTarget(DecorMarker& m);
//
//private:
//    bool m_enabled;
//
//    std::vector<DecorMarker> m_markers;
//
//    // 対象 -> markers index（重複生成防止）
//    std::unordered_map<Object*, size_t> m_indexByTarget;
//
//    // 種別ごとのテクスチャ
//    std::string m_texRibbon;
//    std::string m_texWarning;
//    std::string m_texAnchor;
//};

#pragma once
#include <vector>
#include <unordered_map>
#include <string>

#include "Object.h"

// ColliderTag を「前方宣言」できない（enum の実体が必要）ので
// あなたのプロジェクトで ColliderTag が定義されているヘッダを include してください。
// 例：CollisionManager.h に enum があるならそれ。
// もし別なら、ColliderTag が定義されているヘッダ名に変更してください。
#include "CollisionManager.h"   // <- ColliderTag がここにある前提

// -------------------------
// マーク種別
// -------------------------
enum class DecorMarkType
{
    Ribbon = 0,
    Warning,
    Anchor
};

// -------------------------
// Decorated 通知（イベント）を受ける側
// -------------------------
class IDecorEventListener
{
public:
    virtual ~IDecorEventListener() {}
    virtual void OnDecorated(Object* target, ColliderTag tag) = 0;
    virtual void OnUndecorated(Object* target) = 0;
    virtual void OnTargetDestroyed(Object* target) = 0;
};

// -------------------------
// マークの実体
// -------------------------
struct DecorMarker
{
    Object        icon;
    Object*       target;   // 追従対象
    DecorMarkType type;
    ColliderTag   tag;

    float         offsetX;
    float         offsetY;
    float         sizeW;
    float         sizeH;

    bool          active;

    DecorMarker()
        : target(nullptr)
        , type(DecorMarkType::Ribbon) // 仮の初期値（active=false なので問題にならない）
        , tag((ColliderTag)0)         // ColliderTag に None が無いので 0 キャストにする
        , offsetX(0.0f)
        , offsetY(0.0f)
        , sizeW(32.0f)
        , sizeH(32.0f)
        , active(false)
    {}
};

// -------------------------
// マーク管理
// -------------------------
class DecorMarkSystem : public IDecorEventListener
{
public:
    DecorMarkSystem();
    ~DecorMarkSystem();

    void Init();
    void Update(float dt);
    void Draw();
    void Clear();

    void AddMarker(Object* target, ColliderTag tag);
    void RemoveMarker(Object* target);

    virtual void OnDecorated(Object* target, ColliderTag tag) override;
    virtual void OnUndecorated(Object* target) override;
    virtual void OnTargetDestroyed(Object* target) override;

    void SetEnabled(bool enabled) { m_enabled = enabled; }

    void SetTextureFor(DecorMarkType type, const std::string& path);

private:
    DecorMarkType DecideMarkType(ColliderTag tag) const;
    const char*   GetTexturePath(DecorMarkType type) const;

    void ApplyIconVisual(DecorMarker& m);
    void FollowTarget(DecorMarker& m);

private:
    bool m_enabled;

    std::vector<DecorMarker> m_markers;
    std::unordered_map<Object*, size_t> m_indexByTarget;

    std::string m_texRibbon;
    std::string m_texWarning;
    std::string m_texAnchor;
};

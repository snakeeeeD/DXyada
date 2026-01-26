#pragma once
#include <unordered_map>
#include <string>
#include <DirectXMath.h>
#include "Renderer.h"
#include "Texture.h"

enum class Tag
{
    None,
    Player,
    Enemy,
    Platform
};

enum class UVMode
{
    Sprite, // 通常のスプライトアニメーション
    Loop    // UVループ（リボンなどのスクロール用）
};

struct Animation
{
    std::string name;
    ID3D11ShaderResourceView* srv = nullptr;
    int splitX = 1;
    int splitY = 1;
    int row = 0;
    int startFrame = 0;
    int endFrame = 0;
    float fps = 0.0f;
    bool loop = true;
    bool wrap = true;
    int priority = 0;
};

class Object
{
public:
    HRESULT Init();
    void    Update(float deltaTime);
    void    Draw(
        ID3D11DeviceContext* context,
        ID3D11InputLayout* layout,
        ID3D11VertexShader* vs,
        ID3D11PixelShader* ps,
        ID3D11Buffer* constantBuffer
    );
    void    UnInit();

    // テクスチャ/アニメーション関連
    void AddTexture(const char* texturePath);
    void AddAnimation(
        const std::string& name,
        const char* texturePath,
        int splitX, int splitY,
        int row,
        int startFrame, int endFrame,
        float fps,
        bool loop,
        bool wrap,
        int priority
    );
    void PlayAnimation(const std::string& name);
    void SetCurrentAnimationName(const std::string& name);

    // 基本的なセッター
    void SetPos(float x, float y, float z);
    void SetSize(float x, float y, float z);
    void SetAngle(float a);
    void SetPivot(float x, float y, float z);
    void SetColor(float r, float g, float b, float a);
    void SetFlipX(bool flip) { m_flipX = flip; }

    void SetUVLoop(float length, float scroll)
    {
        m_uvMode = UVMode::Loop;
        m_uvLength = length;
        m_uvScroll = scroll;
    }

    void SetUVSprite()
    {
        m_uvMode = UVMode::Sprite;
    }

    UVMode GetUVMode() const { return m_uvMode; }

    // ゲッター
    DirectX::XMFLOAT3 GetPos() const { return m_pos; }
    DirectX::XMFLOAT3 GetSize() const { return m_size; }
    float GetAngle() const { return m_angle; }
    DirectX::XMFLOAT4 GetColor() const { return m_color; }
    DirectX::XMFLOAT3 GetPivot() const { return m_pivot; }
    bool GetFlipX() const { return m_flipX; }

    float GetUVLength() const { return m_uvLength; }
    float GetUVScroll() const { return m_uvScroll; }

private:
    ID3D11Buffer* m_pVertexBuffer = nullptr;

    DirectX::XMFLOAT3 m_pos = { 0, 0, 0 };
    DirectX::XMFLOAT3 m_size = { 1, 1, 1 };
    DirectX::XMFLOAT3 m_pivot = { 0, 0, 0 };
    float m_angle = 0.0f;
    DirectX::XMFLOAT4 m_color = { 1, 1, 1, 1 };

    std::unordered_map<std::string, Animation> m_animations;
    std::string m_currentAnim;
    int m_currentFrame = 0;
    float m_animTime = 0.0f;

    bool m_flipX = false;

    // UVループ用
    UVMode m_uvMode = UVMode::Sprite;
    float m_uvLength = 1.0f;
    float m_uvScroll = 0.0f;
};
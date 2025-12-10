#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <map>
#include "Renderer.h"
#include "Texture.h"

enum class Tag { None, Player, Enemy, Platform };

struct Animation {
    std::string name;
    int row;
    int startFrame;
    int endFrame;
    float fps;
    bool loop;
    int priority;
};

class Object {
private:
    Vertex m_vertexList[4] = {
        { -0.5f,  0.5f, 0.5f, 1,1,1,1, 0,0 },
        {  0.5f,  0.5f, 0.5f, 1,1,1,1, 1,0 },
        { -0.5f, -0.5f, 0.5f, 1,1,1,1, 0,1 },
        {  0.5f, -0.5f, 0.5f, 1,1,1,1, 1,1 },
    };

    DirectX::XMFLOAT3 m_pos = { 0,0,0 };
    DirectX::XMFLOAT3 m_size = { 100,100,0 };
    float m_angle = 0.0f;
    DirectX::XMFLOAT4 m_color = { 1,1,1,1 };

    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11ShaderResourceView* m_pTextureView = nullptr;

    int m_splitX = 1;
    int m_splitY = 1;

    // Animation
    std::map<std::string, Animation> m_animations;
    std::string m_currentAnim;
    float m_animTime = 0.0f;
    int m_currentFrame = 0;

    DirectX::XMFLOAT3 m_pivot = { 0, 0, 0 };

public:
    float numU = 0;
    float numV = 0;


    HRESULT Init(const char* imgname, int sx = 1, int sy = 1);
    void Update(float deltaTime);
    void UnInit();

    void AddAnimation(const Animation& anim);
    void PlayAnimation(const std::string& name);

    void SetPos(float x, float y, float z);
    void SetSize(float x, float y, float z);
    void SetAngle(float a);
    void SetColor(float r, float g, float b, float a);


    DirectX::XMFLOAT3 GetPos() const;
    DirectX::XMFLOAT3 GetSize() const;

    float GetAngle();
    DirectX::XMFLOAT4 GetColor();

    Texture* m_texture = nullptr;

    void Draw(
        ID3D11DeviceContext* context,
        ID3D11InputLayout* layout,
        ID3D11VertexShader* vs,
        ID3D11PixelShader* ps,
        ID3D11Buffer* constantBuffer
    );

    void SetPivot(float x, float y, float z) { m_pivot = { x, y, z }; }

};

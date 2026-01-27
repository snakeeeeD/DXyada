// Object.cpp
#include "Object.h"
#include "Camera2D.h" // g_cameraPos をここから参照している想定（なければ extern を別へ）

using namespace DirectX;

HRESULT Object::Init()
{
    // すでに作っていたら再生成しない
    if (m_pVertexBuffer) return S_OK;

    Vertex vertices[4] =
    {
        { -0.5f,  0.5f, 0.5f, 1,1,1,1, 0,0 },
        {  0.5f,  0.5f, 0.5f, 1,1,1,1, 1,0 },
        { -0.5f, -0.5f, 0.5f, 1,1,1,1, 0,1 },
        {  0.5f, -0.5f, 0.5f, 1,1,1,1, 1,1 },
    };

    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth = sizeof(vertices);
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA sd{};
    sd.pSysMem = vertices;

    return g_pDevice->CreateBuffer(&bd, &sd, &m_pVertexBuffer);
}

void Object::Update(float deltaTime)
{
    if (m_currentAnim.empty()) return;

    Animation& anim = m_animations[m_currentAnim];

    // fps=0（静止画）対策：Updateで0除算しない
    if (anim.fps <= 0.0f) return;

    m_animTime += deltaTime;

    const float frameTime = 1.0f / anim.fps;
    while (m_animTime >= frameTime)
    {
        m_animTime -= frameTime;
        m_currentFrame++;

        if (m_currentFrame > anim.endFrame)
        {
            if (anim.loop)
                m_currentFrame = anim.startFrame;
            else
                m_currentFrame = anim.endFrame;
        }
    }
}

void Object::Draw(
    ID3D11DeviceContext* context,
    ID3D11InputLayout* layout,
    ID3D11VertexShader* vs,
    ID3D11PixelShader* ps,
    ID3D11Buffer* constantBuffer
)
{
    if (m_currentAnim.empty()) return;

    auto it = m_animations.find(m_currentAnim);
    if (it == m_animations.end()) return;

    Animation& anim = it->second;
    if (!anim.srv) return;
    if (anim.splitX <= 0 || anim.splitY <= 0) return;
    if (!m_pVertexBuffer) return;

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    context->IASetInputLayout(layout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    context->VSSetShader(vs, nullptr, 0);
    context->PSSetShader(ps, nullptr, 0);
    context->PSSetShaderResources(0, 1, &anim.srv);

    if (m_uvMode == UVMode::Loop)
        context->PSSetSamplers(0, 1, &g_pSamplerWrap);
    else
        context->PSSetSamplers(0, 1, &g_pSamplerClamp);

    // ===== ワールド行列 =====
    XMMATRIX matScale = XMMatrixScaling(m_size.x, m_size.y, m_size.z);
    XMMATRIX matRot = XMMatrixRotationZ(XMConvertToRadians(m_angle));
    XMMATRIX matTrans = XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);

    XMMATRIX matPivot = XMMatrixTranslation(m_pivot.x, m_pivot.y, m_pivot.z);
    XMMATRIX matNegPivot = XMMatrixTranslation(-m_pivot.x, -m_pivot.y, -m_pivot.z);

    XMMATRIX matWorld =
        matScale *
        matNegPivot *
        matRot *
        matPivot *
        matTrans;

    XMMATRIX matView =
        XMMatrixTranslation(-g_cameraPos.x, -g_cameraPos.y, 0.0f);

    XMMATRIX matProj =
        XMMatrixOrthographicLH((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, 3.0f);

    // ===== UV 行列 =====
    int frame = m_currentFrame;
    if (frame < anim.startFrame) frame = anim.startFrame;
    if (frame > anim.endFrame)   frame = anim.endFrame;

    const float sx = 1.0f / (float)anim.splitX;
    const float sy = 1.0f / (float)anim.splitY;

    int currentRow = anim.row;
    int currentCol = frame;

    if (anim.wrap)
    {
        currentRow = frame / anim.splitX;
        currentCol = frame % anim.splitX;
    }

    const float u = currentCol * sx;
    const float v = currentRow * sy;

    XMMATRIX matTex = XMMatrixIdentity();

    if (m_uvMode == UVMode::Sprite)
    {
        if (!m_flipX)
        {
            matTex =
                XMMatrixScaling(sx, sy, 1.0f) *
                XMMatrixTranslation(u, v, 0.0f);
        }
        else
        {
            matTex =
                XMMatrixScaling(-sx, sy, 1.0f) *
                XMMatrixTranslation(u + sx, v, 0.0f);
        }
    }
    else if (m_uvMode == UVMode::Loop)
    {
        matTex =
            XMMatrixScaling(m_uvLength, 1.0f, 1.0f) *
            XMMatrixTranslation(m_uvScroll, 0.0f, 0.0f);
    }
    else // UVMode::Gauge
    {
        // 左端固定、右端だけ 0..m_uvCrop の範囲だけ表示
        // ＝ 横方向を m_uvCrop 倍にスケール（u=0起点）
        matTex =
            XMMatrixScaling(m_uvCrop, 1.0f, 1.0f);
    }

    // ===== 定数バッファ =====
    ConstBuffer cb{};
    cb.matrixWorld = XMMatrixTranspose(matWorld);
    cb.matrixView = XMMatrixTranspose(matView);
    cb.matrixProj = XMMatrixTranspose(matProj);
    cb.matrixTex = XMMatrixTranspose(matTex);
    cb.color = m_color;

    context->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);
    context->Draw(4, 0);
}

void Object::AddTexture(const char* texturePath)
{
    AddAnimation(
        "DefaultTexture",
        texturePath,
        1, 1,
        0,
        0, 0,
        0.0f,   // 静止画なので fps=0 でOK（Update側で0除算回避済み）
        false,
        false,
        -100
    );

    if (m_currentAnim.empty())
        PlayAnimation("DefaultTexture");
}

void Object::AddAnimation(
    const std::string& name,
    const char* texturePath,
    int splitX, int splitY,
    int row,
    int startFrame, int endFrame,
    float fps,
    bool loop,
    bool wrap,
    int priority
)
{
    // 同名上書き時の SRV リーク防止
    auto it = m_animations.find(name);
    if (it != m_animations.end())
    {
        SAFE_RELEASE(it->second.srv);
    }

    Animation anim{};
    anim.name = name;
    anim.splitX = splitX;
    anim.splitY = splitY;
    anim.row = row;
    anim.startFrame = startFrame;
    anim.endFrame = endFrame;
    anim.fps = fps;
    anim.loop = loop;
    anim.wrap = wrap;
    anim.priority = priority;

    // TextureManager 側でキャッシュ（AddRef含む設計が前提）
    anim.srv = TextureManager::Instance().GetSRV(g_pDevice, texturePath);

    m_animations[name] = anim;
}

void Object::PlayAnimation(const std::string& name)
{
    auto it = m_animations.find(name);
    if (it == m_animations.end()) return;

    m_currentAnim = name;
    m_currentFrame = it->second.startFrame;
    m_animTime = 0.0f;
}

void Object::SetCurrentAnimationName(const std::string& name)
{
    auto it = m_animations.find(name);
    if (it == m_animations.end()) return;

    m_currentAnim = name;
}

void Object::UnInit()
{
    if (m_uninitCheck)
    {
        return;
    }
    SAFE_RELEASE(m_pVertexBuffer);

    for (auto& kv : m_animations)
    {
        SAFE_RELEASE(kv.second.srv);
    }
    m_animations.clear();

    m_currentAnim.clear();
    m_currentFrame = 0;
    m_animTime = 0.0f;

    m_uninitCheck = true;
}

void Object::SetPos(float x, float y, float z) { m_pos = { x, y, z }; }
void Object::SetSize(float x, float y, float z) { m_size = { x, y, z }; }
void Object::SetAngle(float a) { m_angle = a; }
void Object::SetPivot(float x, float y, float z) { m_pivot = { x, y, z }; }
void Object::SetColor(float r, float g, float b, float a) { m_color = { r, g, b, a }; }

void Object::ChangeTexture(const char* texturePath)
{
    AddAnimation("Decoration", texturePath, 1, 1, 0, 0, 0, 0.0f, false, false, 999);
    PlayAnimation("Decoration");
}

void Object::SetUVMode(UVMode m) { m_uvMode = m; }
void Object::SetUVLength(float len) { m_uvLength = len; }
void Object::SetUVScroll(float scroll) { m_uvScroll = scroll; }

void Object::SetUVCrop(float uMax01)
{
    if (uMax01 < 0.0f) uMax01 = 0.0f;
    if (uMax01 > 1.0f) uMax01 = 1.0f;
    m_uvCrop = uMax01;
}
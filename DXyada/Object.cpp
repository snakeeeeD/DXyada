#include "Object.h"
#include "Camera2D.h"
using namespace DirectX;

HRESULT Object::Init()
{
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
    m_animTime += deltaTime;

    float frameTime = 1.0f / anim.fps;
    while (m_animTime >= frameTime)
    {
        m_animTime -= frameTime;
        m_currentFrame++;

        if (m_currentFrame > anim.endFrame)
        {
            if (anim.loop)
            {
                m_currentFrame = anim.startFrame;
            }   
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

    Animation& anim = m_animations[m_currentAnim];
    if (!anim.srv) return;
    if (anim.splitX <= 0 || anim.splitY <= 0) return;

    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    context->IASetInputLayout(layout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    context->VSSetShader(vs, nullptr, 0);
    context->PSSetShader(ps, nullptr, 0);
    context->PSSetShaderResources(0, 1, &anim.srv);

    if (m_uvMode == UVMode::Loop)
    {
        context->PSSetSamplers(0, 1, &g_pSamplerWrap);
    }
    else
    {
        context->PSSetSamplers(0, 1, &g_pSamplerClamp);
    }

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
        XMMatrixOrthographicLH(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 3.0f);

    // ===== UV 行列（ここが最重要）=====
    //int frame = m_currentFrame;
    //if (frame < anim.startFrame) frame = anim.startFrame;
    //if (frame > anim.endFrame)   frame = anim.endFrame;

    //float sx = 1.0f / anim.splitX;
    //float sy = 1.0f / anim.splitY;

    //float u = frame * sx;
    //float v = anim.row * sy;

    //XMMATRIX matTex =
    //    XMMatrixScaling(sx, sy, 1.0f) *
    //    XMMatrixTranslation(u, v, 0.0f);

    int frame = m_currentFrame;
    if (frame < anim.startFrame) frame = anim.startFrame;
    if (frame > anim.endFrame)   frame = anim.endFrame;

    float sx = 1.0f / anim.splitX;
    float sy = 1.0f / anim.splitY;

    int currentRow = anim.row;
    int currentCol = frame;

    if (anim.wrap)
    {
        currentRow = frame / anim.splitX;  //行番号
        currentCol = frame % anim.splitX;  //列番号
    }

    float u = currentCol * sx;
    float v = currentRow * sy;

    XMMATRIX matTex;

    if (m_uvMode == UVMode::Sprite)
    {
        if (!m_flipX)
        {
            //通常
            matTex =
                XMMatrixScaling(sx, sy, 1.0f) *
                XMMatrixTranslation(u, v, 0.0f);
        }
        else
        {
            //左右反転
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
        0.0f,
        false,
        false,
        -100   
    );

    if (m_currentAnim.empty())
    {
        PlayAnimation("DefaultTexture");
    }
}

/*
* @param    splitX : 横の分割数 
* @param    splitY : 縦の分割数
* @param    row : 使用する行
* @param    startFrame : 開始フレーム
* @param    endFrame :  終了フレーム
* @param    fps : フレームレート
* @param    loop : ループするかどうか
* @param    wrap : 横列が最後まで行ったとき改行するかどうか
* @param    priority : 優先度(数字が高いほど優先度高)
*/
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
    Animation anim{};
    anim.name = name;
    anim.splitX = splitX;
    anim.splitY = splitY;
    anim.row = row;
    anim.startFrame = startFrame;
    anim.endFrame = endFrame;
    anim.fps = fps;
    anim.loop = loop;
    anim.wrap = wrap,
    anim.priority = priority;

    Texture* tex = new Texture();
    tex->SetSpriteSheet(splitX, splitY);
    tex->Load(g_pDevice, texturePath);
    anim.srv = tex->GetSRV();

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

//左右反転時、名前のみ返す
void Object::SetCurrentAnimationName(const std::string& name)
{
    auto it = m_animations.find(name);
    if (it == m_animations.end()) return;


    m_currentAnim = name;
}

void Object::UnInit()
{
    SAFE_RELEASE(m_pVertexBuffer);

    for (auto& kv : m_animations)
    {
        SAFE_RELEASE(kv.second.srv);
    }
    m_animations.clear();
}

void Object::SetPos(float x, float y, float z) { m_pos = { x,y,z }; }
void Object::SetSize(float x, float y, float z) { m_size = { x,y,z }; }
void Object::SetAngle(float a) { m_angle = a; }
void Object::SetPivot(float x, float y, float z) { m_pivot = { x,y,z }; }
void Object::SetColor(float r, float g, float b, float a) { m_color = { r,g,b,a }; }

void Object::ChangeTexture(const char* texturePath)
{
    AddAnimation("Decoration", texturePath, 1, 1, 0, 0, 0, 1.0f, false, false, 999);

    PlayAnimation("Decoration");
}

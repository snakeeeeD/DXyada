#include "Object.h"
#include"Camera2D.h"
using namespace DirectX;
// 初期化
HRESULT Object::Init(const char* imgname, int sx, int sy) {
    m_splitX = sx;
    m_splitY = sy;

    // UV 設定
    m_vertexList[1].u = 1.0f / m_splitX;
    m_vertexList[2].v = 1.0f / m_splitY;
    m_vertexList[3].u = 1.0f / m_splitX;
    m_vertexList[3].v = 1.0f / m_splitY;


    // 頂点バッファ作成
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = sizeof(m_vertexList);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA subResource{};
    subResource.pSysMem = m_vertexList;

    HRESULT hr = g_pDevice->CreateBuffer(&bufferDesc, &subResource, &m_pVertexBuffer);
    if (FAILED(hr)) return hr;

    // Texture を保持する
    m_texture = new Texture();
    m_texture->SetSpriteSheet(m_splitX, m_splitY);

    if (!m_texture->Load(g_pDevice, imgname)) {
        MessageBoxA(NULL, "テクスチャ読込失敗", "エラー", MB_ICONERROR | MB_OK);
        delete m_texture;
        m_texture = nullptr;
        return E_FAIL;
    }

    // SRV を保持
    m_pTextureView = m_texture->GetSRV();

    return S_OK;
}

// 描画
void Object::Draw(
    ID3D11DeviceContext* context,
    ID3D11InputLayout* layout,
    ID3D11VertexShader* vs,
    ID3D11PixelShader* ps,
    ID3D11Buffer* constantBuffer

) {
    // --- リソースチェック ---
    if (!context) { MessageBoxA(NULL, "DeviceContext NULL", "Error", MB_OK); return; }
    if (!layout) { MessageBoxA(NULL, "InputLayout NULL", "Error", MB_OK); return; }
    if (!vs) { MessageBoxA(NULL, "VertexShader NULL", "Error", MB_OK); return; }
    if (!ps) { MessageBoxA(NULL, "PixelShader NULL", "Error", MB_OK); return; }
    if (!constantBuffer) { MessageBoxA(NULL, "ConstantBuffer NULL", "Error", MB_OK); return; }
    if (!m_pVertexBuffer) { MessageBoxA(NULL, "VertexBuffer NULL", "Error", MB_OK); return; }
    if (!m_pTextureView) { MessageBoxA(NULL, "Texture NULL", "Error", MB_OK); return; }

    // --- GPU パイプラインにリソースをセット ---
    context->IASetInputLayout(layout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    context->VSSetShader(vs, nullptr, 0);
    context->PSSetShader(ps, nullptr, 0);
    context->PSSetShaderResources(0, 1, &m_pTextureView);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    // --- 定数バッファの更新 ---
    ConstBuffer cb{};
    DirectX::XMMATRIX matrixProj = DirectX::XMMatrixOrthographicLH(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 3.0f);
    DirectX::XMMATRIX matrixView = DirectX::XMMatrixTranslation(-g_cameraPos.x, -g_cameraPos.y, 0.0f);

    XMMATRIX matrixScale = XMMatrixScaling(m_size.x, m_size.y, m_size.z);
    XMMATRIX matrixRot = XMMatrixRotationZ(m_angle * XM_PI / 180.0f);
    XMMATRIX matrixTrans = XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);

    XMMATRIX matPivot = XMMatrixTranslation(m_pivot.x, m_pivot.y, m_pivot.z);
    XMMATRIX matNegPivot = XMMatrixTranslation(-m_pivot.x, -m_pivot.y, -m_pivot.z);

    XMMATRIX matrixWorld =
        matrixScale *
        matNegPivot *  
        matrixRot *     
        matPivot *       
        matrixTrans;    


    float u = numU / m_splitX;
    float v = numV / m_splitY;
    DirectX::XMMATRIX matrixTex = DirectX::XMMatrixTranslation(u, v, 0);

    cb.matrixProj = DirectX::XMMatrixTranspose(matrixProj);
    cb.matrixView = DirectX::XMMatrixTranspose(matrixView);
    cb.matrixWorld = DirectX::XMMatrixTranspose(matrixWorld);
    cb.matrixTex = DirectX::XMMatrixTranspose(matrixTex);
    cb.color = m_color;

    context->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);

    //カメラ動作


    // --- 描画 ---
    context->Draw(4, 0);
}


// 更新（アニメーション）
void Object::Update(float deltaTime) {
    if (m_currentAnim.empty()) return;

    Animation& anim = m_animations[m_currentAnim];
    m_animTime += deltaTime;

    float frameDuration = 1.0f / anim.fps;
    while (m_animTime >= frameDuration) {
        m_animTime -= frameDuration;
        m_currentFrame++;
        if (m_currentFrame > anim.endFrame) {
            if (anim.loop) m_currentFrame = anim.startFrame;
            else m_currentFrame = anim.endFrame;
        }
    }

    numU = m_currentFrame;
    numV = anim.row;
}

// アニメーション追加
void Object::AddAnimation(const Animation& anim) {
    m_animations[anim.name] = anim;
}

// アニメーション再生
void Object::PlayAnimation(const std::string& name) {
    if (m_currentAnim == name) return;
    if (m_animations.find(name) != m_animations.end()) {
        Animation& newAnim = m_animations[name];
        Animation* current = m_animations.count(m_currentAnim) ? &m_animations[m_currentAnim] : nullptr;

        if (!current || newAnim.priority >= current->priority) {
            m_currentAnim = name;
            m_currentFrame = newAnim.startFrame;
            m_animTime = 0.0f;
        }
    }
}

// 開放
void Object::UnInit() {
    SAFE_RELEASE(m_pVertexBuffer);
    // SRV の Release は Texture がやる
    if (m_texture) {
        delete m_texture;
        m_texture = nullptr;
    }
}



// セッター・ゲッター
void Object::SetPos(float x, float y, float z) { m_pos = { x,y,z }; }
void Object::SetSize(float x, float y, float z) { m_size = { x,y,z }; }
void Object::SetAngle(float a) { m_angle = a; }
void Object::SetColor(float r, float g, float b, float a) { m_color = { r,g,b,a }; }
DirectX::XMFLOAT3 Object::GetPos() const { return m_pos; }
DirectX::XMFLOAT3 Object::GetSize() const { return m_size; }
float Object::GetAngle() { return m_angle; }
DirectX::XMFLOAT4 Object::GetColor() { return m_color; }

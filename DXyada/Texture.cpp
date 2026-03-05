#include "Texture.h"
#include "stb_image.h"
#include <windows.h>

Texture::Texture()
    : m_srv(nullptr), m_cols(1), m_rows(1), m_frameWidth(0), m_frameHeight(0)
{}

Texture::~Texture()
{
    if (m_srv) m_srv->Release();
}

// 画像読み込み
bool Texture::Load(ID3D11Device* device, const char* filename)
{
    unsigned char* pixels = nullptr;
    int width, height, bpp;

    pixels = stbi_load(filename, &width, &height, &bpp, 4);
    if (!pixels) 
    {
        MessageBoxA(NULL, filename, "Texture Load Error", MB_OK);
        char buf[1024];
        sprintf_s(buf, "[TextureLoadError] %s\n", filename);  // pathは失敗した文字列
        OutputDebugStringA(buf);
        return false;
    }

    m_frameWidth = width / m_cols;
    m_frameHeight = height / m_rows;

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA subResource{};
    subResource.pSysMem = pixels;
    subResource.SysMemPitch = width * 4;

    ID3D11Texture2D* pTexture = nullptr;
    HRESULT hr = device->CreateTexture2D(&desc, &subResource, &pTexture);
    stbi_image_free(pixels);
    if (FAILED(hr)) return false;

    hr = device->CreateShaderResourceView(pTexture, nullptr, &m_srv);
    pTexture->Release();

    return SUCCEEDED(hr);
}

// スプライトシート分割設定
void Texture::SetSpriteSheet(int cols, int rows)
{
    m_cols = cols;
    m_rows = rows;
}

// 指定フレームの UV を取得
void Texture::GetFrameUV(int frame, float& u0, float& v0, float& u1, float& v1, bool flipX) const
{
    int col = frame % m_cols;
    int row = frame / m_cols;

    u0 = (float)col / m_cols;
    v0 = (float)row / m_rows;
    u1 = (float)(col + 1) / m_cols;
    v1 = (float)(row + 1) / m_rows;

    //左右反転
    if (flipX)
    {
        float temp = u0;
        u0 = u1;
        u1 = temp;
    }
}

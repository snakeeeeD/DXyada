#pragma once
#include <d3d11.h>

class Texture
{
public:
    Texture();
    ~Texture();

    bool Load(ID3D11Device* device, const char* filename);

    void SetSpriteSheet(int cols, int rows); // スプライトシート分割設定

    // frame 番号から UV を取得
    void GetFrameUV(int frame, float& u0, float& v0, float& u1, float& v1) const;

    ID3D11ShaderResourceView* GetSRV() const { return m_srv; }

    int GetCols() const { return m_cols; }
    int GetRows() const { return m_rows; }
    int GetFrameWidth() const { return m_frameWidth; }
    int GetFrameHeight() const { return m_frameHeight; }

private:
    ID3D11ShaderResourceView* m_srv;
    int m_cols;
    int m_rows;
    int m_frameWidth;
    int m_frameHeight;
};

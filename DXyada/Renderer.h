#pragma once
#include <d3d11.h>  // DirectX11を使うためのヘッダーファイル
#include<DirectXMath.h>//数学関連のヘッダファイル

#define SCREEN_WIDTH (1920)	// ウインドウの幅
#define SCREEN_HEIGHT (1080)	// ウインドウの高さ

// 関数のプロトタイプ宣言
HRESULT RendererInit(HWND hwnd);
void    RendererDrawStart();
void    RendererDrawEnd();
void    RendererUninit();
HRESULT ResizeWindow(int width, int height);

// 構造体の定義
extern ID3D11InputLayout* g_pInputLayout;
extern ID3D11VertexShader* g_pVertexShader;
extern ID3D11PixelShader* g_pPixelShader;

// 頂点データを表す構造体
struct Vertex
{

	float x, y, z;// 頂点の位置座標
	float r, g, b, a;//色（赤・緑・青・アルファ）
	float u, v;//テクスチャ座標（UV座標）
};

//定数バッファ用構造体
struct ConstBuffer
{
	//頂点カラー行列
	DirectX::XMFLOAT4 color;
	//UV座標行列
	DirectX::XMMATRIX matrixTex;
	//プロジェクション変換行列
	DirectX::XMMATRIX matrixProj;
	//ワールド変換行列
	DirectX::XMMATRIX matrixWorld;
	//View行列
	DirectX::XMMATRIX matrixView;
};

// デバイス＝DirectXの各種機能を作る ※ID3D11で始まるポインタ型の変数は、解放する必要がある
extern ID3D11Device* g_pDevice;
// コンテキスト＝描画関連を司る機能
extern ID3D11DeviceContext* g_pDeviceContext;
extern ID3D11Buffer* g_pConstantBuffer;

extern IDXGISwapChain* g_pSwapChain;
// Direct3D解放の簡略化マクロ
#define SAFE_RELEASE(p) { if( NULL != p ) { p->Release(); p = NULL; } }
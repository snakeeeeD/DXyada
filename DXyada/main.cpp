#undef UNICODE  // Unicodeではなく、マルチバイト文字を使う
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include"Renderer.h"
#include"Game.h"


// マクロ定義
#define CLASS_NAME   "DX21Smpl"// ウインドウクラスの名前
#define WINDOW_NAME  "DirectX初期化"// ウィンドウの名前

// 関数のプロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);





//--------------------------------------------------------------------------------------
// エントリポイント＝一番最初に実行される関数
//--------------------------------------------------------------------------------------
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// ウィンドウクラス情報をまとめる
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	extern IDXGISwapChain* g_pSwapChain;

	// ウィンドウの情報をまとめる
	HWND hWnd;
	hWnd = CreateWindowEx(0,	// 拡張ウィンドウスタイル
		CLASS_NAME,				// ウィンドウクラスの名前
		WINDOW_NAME,			// ウィンドウの名前
		WS_OVERLAPPEDWINDOW,	// ウィンドウスタイル
		CW_USEDEFAULT,			// ウィンドウの左上Ｘ座標
		CW_USEDEFAULT,			// ウィンドウの左上Ｙ座標 
		SCREEN_WIDTH,			// ウィンドウの幅
		SCREEN_HEIGHT,			// ウィンドウの高さ
		NULL,					// 親ウィンドウのハンドル
		NULL,					// メニューハンドルまたは子ウィンドウID
		hInstance,				// インスタンスハンドル
		NULL);					// ウィンドウ作成データ

	//ウィンドウのサイズを変更
	RECT rc1, rc2;
	GetWindowRect(hWnd, &rc1);
	GetClientRect(hWnd, &rc2);
	int sx = SCREEN_WIDTH;
	int sy = SCREEN_HEIGHT;
	sx += ((rc1.right - rc1.left) - (rc2.right - rc2.left));
	sy += ((rc1.bottom - rc1.top) - (rc2.bottom - rc2.top));
	SetWindowPos(hWnd, NULL, 0, 0, sx, sy, (SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE));
	// 指定されたウィンドウの表示状態を設定(ウィンドウを表示)
	ShowWindow(hWnd, nCmdShow);
	// ウィンドウの状態を直ちに反映(ウィンドウのクライアント領域を更新)
	UpdateWindow(hWnd);

	//ゲーム初期化
	Game game;
	game.Init(hWnd);



	MSG msg;

	int fpsCounter = 0;
	long long oldTick = GetTickCount64();
	long long nowTick = oldTick;

	LARGE_INTEGER liWork;
	long long frequency;
	QueryPerformanceFrequency(&liWork);
	frequency = liWork.QuadPart;

	QueryPerformanceCounter(&liWork);
	long long oldCount = liWork.QuadPart;
	long long nowCount = oldCount;

	
	
	//SetWindowStyleMode(1);
	// ゲームループ
	while (1)
	{
		
		
		// 新たにメッセージがあれば
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// ウィンドウプロシージャにメッセージを送る
			DispatchMessage(&msg);

			// 「WM_QUIT」メッセージを受け取ったらループを抜ける
			if (msg.message == WM_QUIT) {
				break;
			}
		}
		else {
			QueryPerformanceCounter(&liWork);
			nowCount = liWork.QuadPart;
			if (nowCount >= oldCount + frequency / 240) {

				game.Update();
				game.Draw();
				fpsCounter++;
				oldCount = nowCount;
			}
			nowTick = GetTickCount64();

			if (nowTick >= oldTick + 1000) {
				char str[32];
				wsprintfA(str, "FPS=%d", fpsCounter);
				SetWindowTextA(hWnd, str);
				
				fpsCounter = 0;
				oldTick = nowTick;
			}
		}
		
	}

	//ゲーム終了
	game.UnInit();

	UnregisterClass(CLASS_NAME, hInstance);

	return (int)msg.wParam;
}

//--------------------------------------------------------------------------------------
//ウィンドウプロシージャ
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static bool isFullscreen = false;
	static bool isMassageBoxShowed = false;
	switch (uMsg)
	{
	case WM_DESTROY:// ウィンドウ破棄のメッセージ
		PostQuitMessage(0);// 「WM_QUIT」メッセージを送る　→　アプリ終了
		break;

	case WM_CLOSE:  // 「x」ボタンが押されたら
	{
		int res = MessageBoxA(NULL, "終了しますか？", "確認", MB_OKCANCEL);
		if (res == IDOK) {
			DestroyWindow(hWnd);  // 「WM_DESTROY」メッセージを送る
		}
	}
	break;

	case WM_KEYDOWN: //キー入力があったメッセージ
		if (LOWORD(wParam) == VK_ESCAPE)
		{ //入力されたキーがESCAPEなら
			//PostMessage(hWnd, WM_CLOSE, wParam, lParam);//「WM_CLOSE」を送る
		}
		else if (LOWORD(wParam) == VK_F11) {
			isFullscreen = !isFullscreen;
			if (isFullscreen) {
				//g_pSwapChain->SetFullscreenState(TRUE, NULL);
				//ShowWindow(hWnd,SW_MAXIMIZE);
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP | WS_MINIMIZEBOX);
				int screenWidth = GetSystemMetrics(SM_CXSCREEN);
				int screenHeight = GetSystemMetrics(SM_CYSCREEN);
				SetWindowPos(hWnd, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
			g_pSwapChain->ResizeBuffers(
				0, screenWidth, screenHeight,
				DXGI_FORMAT_R8G8B8A8_UNORM, 0);
			}
			else {
				//g_pSwapChain->SetFullscreenState(FALSE, NULL);
				//ShowWindow(hWnd, SW_RESTORE);
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
				SetWindowPos(hWnd, HWND_TOP, 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
			}
		}
		break;

	case WM_ACTIVATE:
		if (wParam == WA_INACTIVE) {
			if (isFullscreen && !isMassageBoxShowed); {
				ShowWindow(hWnd, SW_MINIMIZE);
			}
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);


	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED) {
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);
			ResizeWindow(width, height);
		}
		break;
	default:
		// 受け取ったメッセージに対してデフォルトの処理を実行
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return 0;
}


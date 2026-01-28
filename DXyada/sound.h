#pragma once

#include <xaudio2.h>
// サウンドファイル
typedef enum
{
	SOUND_LABEL_BGM000,		// サンプルBGM
	//SOUND_LABEL_BGM001,			// サンプルBGM

	//SOUND_LABEL_SE_LAND,			//着地音
	SOUND_LABEL_SE_Jump,			//通常ジャンプした時
	SOUND_LABEL_SE_MoveA,			//歩く音1
	SOUND_LABEL_SE_MoveB,			//歩く音2
	SOUND_LABEL_SE_Aim,				//エイムアシストが出た時
	SOUND_LABEL_SE_Throw,			//リボンを投げる
	SOUND_LABEL_SE_Assist,			//エイムアシストがヒットした時
	SOUND_LABEL_SE_Decorate,		//飾った時
	SOUND_LABEL_SE_Kiran,			//ジャストで飾った時
	SOUND_LABEL_SE_Damage,			//被ダメ時
	SOUND_LABEL_SE_Fall,			//落下死した時
	SOUND_LABEL_SE_RibbonJump,		//ピンで大ジャンプをした時
	SOUND_LABEL_SE_Pull,			//ブロックを動かした時
	SOUND_LABEL_SE_Ok,				//決定
	SOUND_LABEL_SE_Pause,			//ポーズ画面を開いた時
	SOUND_LABEL_SE_Select,			//カーソルを動かした時
	SOUND_LABEL_SE_Back,			//キャンセルの音
	SOUND_LABEL_SE_Hariyamasan,		//針を投げる時
	SOUND_LABEL_SE_Ribbon_hodoki,	//リボンをほどく時

	//SOUND_LABEL_SE_STAGEOPEN,		//ステージに入る時

	SOUND_LABEL_MAX,
} SOUND_LABEL;
//extern Sound* g_sound;
class Sound {
private:
	// パラメータ構造体
	typedef struct
	{
		LPCSTR filename;	// 音声ファイルまでのパスを設定
		bool bLoop;			// trueでループ。通常BGMはture、SEはfalse。
	} PARAM;

	PARAM m_param[SOUND_LABEL_MAX] =
	{
		// ===== BGM =====
		{"asset/BGM.wav", true},                  // SOUND_LABEL_BGM000

		// ===== SE =====
		{"asset/SE/Jump.wav", false},             // SOUND_LABEL_SE_Jump
		{"asset/SE/MoveA.wav", false},            // SOUND_LABEL_SE_MoveA
		{"asset/SE/MoveB.wav", false},            // SOUND_LABEL_SE_MoveB
		{"asset/SE/Aim.wav", false},              // SOUND_LABEL_SE_Aim
		{"asset/SE/Throw.wav", false},            // SOUND_LABEL_SE_Throw
		{"asset/SE/Assist.wav", false},           // SOUND_LABEL_SE_Assist
		{"asset/SE/Decorate.wav", false},         // SOUND_LABEL_SE_Decorate
		{"asset/SE/Kiran.wav", false},            // SOUND_LABEL_SE_Kiran
		{"asset/SE/Damage.wav", false},           // SOUND_LABEL_SE_Damage
		{"asset/SE/Fall.wav", false},             // SOUND_LABEL_SE_Fall
		{"asset/SE/RibbonJump.wav", false},       // SOUND_LABEL_SE_RibbonJump
		{"asset/SE/Pull.wav", false},			  // SOUND_LABEL_SE_Pull
		{"asset/SE/Ok.wav", false},               // SOUND_LABEL_SE_Ok
		{"asset/SE/Pause.wav", false},            // SOUND_LABEL_SE_Pause
		{"asset/SE/Select.wav", false},           // SOUND_LABEL_SE_Select
		{"asset/SE/Back.wav", false},		      // SOUND_LABEL_SE_Back
		{"asset/SE/Hariyamasan.wav", false},      // SOUND_LABEL_SE_Hariyamasan
		{"asset/SE/Ribbon_hodoki.wav", false},    // SOUND_LABEL_SE_Ribbon_hodoki
	};

	IXAudio2* m_pXAudio2 = NULL;
	IXAudio2MasteringVoice* m_pMasteringVoice = NULL;
	IXAudio2SourceVoice* m_pSourceVoice[SOUND_LABEL_MAX];
	WAVEFORMATEXTENSIBLE m_wfx[SOUND_LABEL_MAX]; // WAVフォーマット
	XAUDIO2_BUFFER m_buffer[SOUND_LABEL_MAX];
	BYTE* m_DataBuffer[SOUND_LABEL_MAX];

	HRESULT FindChunk(HANDLE, DWORD, DWORD&, DWORD&);
	HRESULT ReadChunkData(HANDLE, void*, DWORD, DWORD);

	bool IsPlaying(SOUND_LABEL label);
	float GetDefaultVolume(SOUND_LABEL label);
public:
	// ゲームループ開始前に呼び出すサウンドの初期化処理
	HRESULT Init(void);

	// ゲームループ終了後に呼び出すサウンドの解放処理
	void Uninit(void);

	// 引数で指定したサウンドを再生する
	void Play(SOUND_LABEL label);

	// 引数で指定したサウンドを停止する
	void Stop(SOUND_LABEL label);

	// 引数で指定したサウンドの再生を再開する
	void Resume(SOUND_LABEL label);

};
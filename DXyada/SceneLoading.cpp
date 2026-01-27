#include "SceneLoading.h"
#include "SceneManager.h"

// 既存と同様に外部
extern DirectX::XMFLOAT3 g_cameraPos;

void SceneLoading::Init()
{
    g_cameraPos = { 0,0,0 };

    m_bg.Init();
    m_bg.AddTexture("asset/Field/back.png");
    m_bg.SetPos(0, 0, 0);
    m_bg.SetSize(1920, 1080, 0);

    m_bar.Init();
    m_bar.AddTexture("asset/Field/block.png");
    m_bar.SetPos(-600, -350, 0);
    m_bar.SetSize(0, 40, 0);

    // 追加：回転スピナー
    m_spinner.Init();
    // “回転して見える”画像を推奨（例：cursor.png や loading.png 等）
    m_spinner.AddTexture("asset/UI/cursor.png");
    m_spinner.SetPos(0, 0, 0);        // 画面中央
    m_spinner.SetSize(160, 160, 0);   // 大きさは好みで
    m_spinAngle = 0.0f;
    m_spinner.SetAngle(m_spinAngle);

    m_lastP = 0.0f;
}

void SceneLoading::Update(SceneManager& mgr)
{
    // ★ずっと回転（ロード中でもロード完了でも回転は毎フレーム進む）
    const float dt = 1.0f / 60.0f; // あなたの前提（固定FPS）
    m_spinAngle += m_spinSpeed * dt;

    // 角度が増え続けても良いが、念のため 0..360 に戻す
    if (m_spinAngle >= 360.0f) m_spinAngle -= 360.0f;

    m_spinner.SetAngle(m_spinAngle);

    StageManager& sm = mgr.GetStageManager();

    // 初回フレームでロード開始していないなら開始
    if (!sm.IsLoading() && sm.GetStage() == nullptr)
    {
        int stage = mgr.GetSelectedStage();
        sm.SetWorkPerFrame(1);  // ここは調整可能
        sm.BeginLoadStage(stage);
    }

    // 進める（true=ロード中 / false=完了）
    //bool loading = sm.StepLoadStage();
    bool loading = sm.StepLoadStage_TimeBudget(0.5);

    float p = sm.GetLoadProgress();
    if (p < 0.0f) p = 0.0f;
    if (p > 1.0f) p = 1.0f;
    m_lastP = p;

    // バー更新
    float w = 1200.0f * p;
    m_bar.SetSize(w, 40, 0);

    // 完了したらゲームへ
    if (!loading)
    {
        mgr.ChangeScene(SCENE_GAME);
    }
}

void SceneLoading::Draw()
{
    m_bg.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );

    m_bar.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );

    // 追加：回転描画
    m_spinner.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
}

void SceneLoading::UnInit()
{
    m_bg.UnInit();
    m_bar.UnInit();
    m_spinner.UnInit();
}

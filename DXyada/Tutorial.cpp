#include "Tutorial.h"
#include <cmath>

Tutorial::Tutorial()
    : m_type(Type::Custom)
    , m_showTutorial(true)
    , m_isCheckpoint(true)
    , m_respawnPosition({ 0, 0, 0 })
    , m_isActivated(false)
    , m_triggerRange(400.0f)
    , m_wasPlayerNear(false)
    , m_hasAnimation(false)
    , m_displayAlpha(0.0f)
    , m_fadeSpeed(3.0f)
    , m_hasChangedTexture(false)
    , m_checkpointTexturePath("")
{
}

Tutorial::~Tutorial()
{
    UnInit();
}

void Tutorial::Init(
    const std::string& signTexturePath,
    float posX,
    float posY,
    float width,
    float height,
    Type type,
    bool showTutorial,
    bool isCheckpoint
)
{
    // 看板の初期化
    m_object.Init();

    //通常の看板テクスチャ
    m_object.AddAnimation("Normal",signTexturePath.c_str(),
        1, 1, 0, 0, 0, 0.0f, false, false, 0);

    //if (isCheckpoint)
    //{
    //    m_object.AddAnimation("Activated", m_checkpointTexturePath.c_str(), 
    //        1, 1, 0, 0, 0, 0.0f, false, false, 1);
    //}

    m_object.PlayAnimation("Normal");
    m_object.SetPos(posX, posY, 0);
    m_object.SetSize(width, height, 0);

    m_type = type;
    m_showTutorial = showTutorial;
    m_isCheckpoint = isCheckpoint;

    //デフォルトの復活地点は看板の位置
    m_respawnPosition = { posX, posY + height * 0.5f + 50.0f, 0 };

    m_isActivated = false;
    m_wasPlayerNear = false;
    m_hasChangedTexture = false;
}
void Tutorial::InitTutorialImage(
    const std::string& imagePath,
    float displayWidth,
    float displayHeight,
    float offsetX,
    float offsetY
)
{
    m_tutorialDisplay.Init();

    // テクスチャパスが空でないか確認
    if (!imagePath.empty())
    {
        m_tutorialDisplay.AddTexture(imagePath.c_str());
    }

    m_tutorialDisplay.SetSize(displayWidth, displayHeight, 0);

    auto signPos = m_object.GetPos();
    m_tutorialDisplay.SetPos(signPos.x + offsetX, signPos.y + offsetY, 0);

    m_hasAnimation = false;
    m_displayAlpha = 0.0f;

    // 初期状態は透明
    m_tutorialDisplay.SetColor(1, 1, 1, 0);
}

void Tutorial::InitTutorialAnimation(
    const std::string& imagePath,
    int row,
    int col,
    int startRow,
    int startCol,
    int endRow,
    int endCol,
    bool loop,
    bool wrap,
    int animSpeed,
    float displayWidth,
    float displayHeight,
    float offsetX,
    float offsetY
)
{
    m_tutorialDisplay.Init();

    // テクスチャパスが空でないか確認
    if (!imagePath.empty())
    {
        m_tutorialDisplay.AddAnimation(
            "Tutorial",
            imagePath.c_str(),
            row, col,
            startRow, startCol,
            endRow, endCol,
            loop,
            wrap,
            animSpeed
        );
    }

    m_tutorialDisplay.SetSize(displayWidth, displayHeight, 0);

    auto signPos = m_object.GetPos();
    m_tutorialDisplay.SetPos(signPos.x + offsetX, signPos.y + offsetY, 0);

    m_hasAnimation = true;
    m_displayAlpha = 0.0f;

    // 初期状態は透明
    m_tutorialDisplay.SetColor(1, 1, 1, 0);
}
void Tutorial::Update(float deltaTime, const DirectX::XMFLOAT3& playerPos)
{
    auto pos = m_object.GetPos();

    float dx = playerPos.x - pos.x;
    //プレイヤーとの距離を計算
    if (m_isCheckpoint)
    {
        dx -= 150;
    }
    else
    {
        dx += 80;
    }
    float dy = playerPos.y - pos.y;
    float distance = sqrt(dx * dx + dy * dy);

    bool isPlayerNear = distance < m_triggerRange;

    //プレイヤーが範囲に入った瞬間
    if (isPlayerNear && !m_wasPlayerNear)
    {
        if (m_isCheckpoint)
        {
            //チェックポイントを有効化
            m_isActivated = true;
        }

        //アニメーションがある場合は再生
        if (m_hasAnimation)
        {
            m_tutorialDisplay.PlayAnimation("Tutorial");
        }
    }

    //チュートリアル表示のフェードイン,アウト
    if (m_showTutorial)
    {
        if (isPlayerNear)
        {
            //フェードイン
            m_displayAlpha += m_fadeSpeed * deltaTime;
            if (m_displayAlpha > 1.0f) m_displayAlpha = 1.0f;
        }
        else
        {
            //フェードアウト
            m_displayAlpha -= m_fadeSpeed * deltaTime;
            if (m_displayAlpha < 0.0f) m_displayAlpha = 0.0f;
        }

        m_tutorialDisplay.SetColor(1, 1, 1, m_displayAlpha);
        m_tutorialDisplay.Update(deltaTime);
    }

    m_wasPlayerNear = isPlayerNear;

    ////テクスチャ切り替え
    //if (m_isActivated && !m_hasChangedTexture)
    //{
    //    m_object.PlayAnimation("Activated");
    //    m_hasChangedTexture = true;
    //}
    if (m_isActivated && !m_hasChangedTexture && !m_checkpointTexturePath.empty())
    {
        m_object.AddAnimation("Activated", m_checkpointTexturePath.c_str(),
            1, 1, 0, 0, 0, 0.0f, false, false, 1);

        m_object.PlayAnimation("Activated");
        m_hasChangedTexture = true;

        char buf[256];
        sprintf_s(buf, "Tutorial: Switched to Activated texture: %s\n", m_checkpointTexturePath.c_str());
        OutputDebugStringA(buf);
    }
 

    m_object.Update(deltaTime);
}

void Tutorial::Draw()
{
    m_object.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );

    if (m_showTutorial && m_displayAlpha > 0.01f)
    {
        m_tutorialDisplay.Draw(
            g_pDeviceContext,
            g_pInputLayout,
            g_pVertexShader,
            g_pPixelShader,
            g_pConstantBuffer
        );
    }
}

void Tutorial::UnInit()
{
    m_object.UnInit();
    m_tutorialDisplay.UnInit();
}


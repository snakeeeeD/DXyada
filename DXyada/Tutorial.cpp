#include "Tutorial.h"
#include <cmath>

Tutorial::Tutorial()
    : m_type(Type::Custom)
    , m_showTutorial(true)
    , m_isCheckpoint(true)
    , m_respawnPosition({ 0, 0, 0 })
    , m_isActivated(false)
    , m_triggerRange(150.0f)
    , m_wasPlayerNear(false)
    , m_hasAnimation(false)
    , m_displayAlpha(0.0f)
    , m_fadeSpeed(3.0f)
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
    m_object.AddTexture(signTexturePath.c_str());
    m_object.SetPos(posX, posY, 0);
    m_object.SetSize(width, height, 0);

    m_type = type;
    m_showTutorial = showTutorial;
    m_isCheckpoint = isCheckpoint;

    //デフォルトの復活地点は看板の位置
    m_respawnPosition = { posX, posY + height * 0.5f + 50.0f, 0 };

    m_isActivated = false;
    m_wasPlayerNear = false;
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

    //プレイヤーとの距離を計算
    float dx = playerPos.x - pos.x;
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

    //仮で色を変える
    if (m_isActivated)
    {
       m_object.SetColor(0.5f, 1.0f, 0.5f, 1.0f);  //緑
    }
    else if (isPlayerNear)
    {
        m_object.SetColor(1.0f, 1.0f, 0.5f, 1.0f);  //黄
    }
    else
    {
        m_object.SetColor(1.0f, 1.0f, 1.0f, 1.0f);  //白
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
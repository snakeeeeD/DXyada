#include "SceneLoad.h"
#include "SceneManager.h"
#include "TextureManager.h"
#include "Renderer.h"
#include "Input.h"
#include "AssetList.h"

#include <cstdlib>
#include <ctime>
#include "sound.h"

extern Sound* g_sound;
extern Input input;

static float Clamp(float v, float a, float b)
{
    if (v < a) return a;
    if (v > b) return b;
    return v;
}

static float EaseOutCubic01(float t)
{
    t = Clamp(t, 0.0f, 1.0f);
    float u = 1.0f - t;
    return 1.0f - (u * u * u);
}

static float EaseInCubic01(float t)
{
    t = Clamp(t, 0.0f, 1.0f);
    return t * t * t;
}

static float UpDown01(float t)
{
    t = Clamp(t, 0.0f, 1.0f);

    if (t < 0.5f)
    {
        float u = t / 0.5f;
        return EaseOutCubic01(u);
    }
    else
    {
        float u = (t - 0.5f) / 0.5f;
        return 1.0f - EaseInCubic01(u);
    }
}

static int RandRange(int a, int b)
{
    return a + (std::rand() % (b - a + 1));
}

void SceneLoad::Init()
{
    static bool seeded = false;
    if (!seeded)
    {
        std::srand((unsigned int)std::time(nullptr));
        seeded = true;
    }

    g_cameraPos = { 0,0,0 };

    m_bg.Init();
    m_bg.AddTexture("asset/UI/背景.png");
    m_bg.SetPos(0, 0, 0);
    m_bg.SetSize(1920, 1080, 1);

    m_done = false;
    m_frame = 0;
    m_previewReady = false;
    m_wasDone = false;
    m_mode = MODE_LOADING;

    m_gaugeFinalW = 600.0f;
    m_gaugeFinalH = 150.0f;
    m_gaugeY = 350.0f;
    m_gaugeT = 0.0f;
    m_gauge01 = 0.0f;
    m_gaugeJustMax = false;

    const float finalCenterX = 0.0f;
    m_gaugeLeftX = finalCenterX - (m_gaugeFinalW * 0.5f);

    m_ribbonGauge.Init();
    m_ribbonGauge.AddTexture("asset/UI/リボンゲージ_満タン.png");
    m_ribbonGauge.SetUVMode(Object::UVMode::Gauge);

    m_ribbonGauge.SetPos(m_gaugeLeftX, m_gaugeY, 0);
    m_ribbonGauge.SetSize(0.0f, m_gaugeFinalH, 0);
    m_ribbonGauge.SetUVCrop(1.0f);

    TextureManager& tm = TextureManager::Instance();
    tm.Enqueue("asset/Field/back.png");
    tm.Enqueue("asset/Player/Player_Idle.png");
    tm.Enqueue("asset/Player/Player_Walk.png");
    tm.Enqueue("asset/Field/block.png");
    tm.Enqueue("asset/Field/Wing_Rippa.png");
    tm.Enqueue("asset/Field/rippa.png");
    tm.Enqueue("asset/Field/needle_floor.png");
    tm.Enqueue("asset/Player/Player_Damage.png");
    tm.Enqueue("asset/Player/Player_SmallJump.png");
    tm.Enqueue("asset/Field/ブロック 1.png");
    tm.Enqueue("asset/Field/ブロック 2.png");
    tm.Enqueue("asset/Field/ブロック 3.png");
    tm.Enqueue("asset/Field/ブロック 4.png");
    tm.Enqueue("asset/Field/ブロック 5.png");

    BuildQueue();

    m_obs.clear();
    m_gameOver = false;
    m_score = 0.0f;

    m_lastType = ObstacleType::WingRippa;
    m_sameCount = 0;

    m_ribbonGauge.SetSize(0.0f, m_gaugeFinalH, 0);
    m_ribbonGauge.SetPos(m_gaugeLeftX, m_gaugeY, 0);

    m_walkSETimer = 0.0f;
    walkchange = 0;
    jumpse = true;
    m_groundUV = 0.0f;
}

void SceneLoad::BuildQueue()
{
    TextureManager& tm = TextureManager::Instance();
    for (size_t i = 0; i < AssetList::AllCount; ++i)
    {
        tm.Enqueue(AssetList::All[i]);
    }
}

void SceneLoad::Update(SceneManager& mgr)
{
    input.Update();
    TextureManager& tm = TextureManager::Instance();
    const float dt = 1.0f / 60.0f;

    m_walkSETimer += dt;
    if (m_mode != MODE_MINI)
    {
        if (m_walkSETimer > 0.5f && walkchange == 0)
        {
            g_sound->Play(SOUND_LABEL_SE_MoveA);
            walkchange = 1;
            m_walkSETimer = 0.0f;
        }
        if (m_walkSETimer > 0.5f && walkchange == 1)
        {
            g_sound->Play(SOUND_LABEL_SE_MoveB);
            walkchange = 0;
            m_walkSETimer = 0.0f;
        }
    }

    m_frame++;

    float prevGauge01 = m_gauge01;

    m_gaugeT += dt;

    const float t0 = 2.8f;
    const float t1 = 2.2f;

    float g = 0.0f;
    if (m_gaugeT <= t0)
    {
        float u = m_gaugeT / t0;
        g = 0.70f * EaseOutCubic01(u);
    }
    else
    {
        float u = (m_gaugeT - t0) / t1;
        u = Clamp(u, 0.0f, 1.0f);
        g = 0.70f + 0.30f * EaseOutCubic01(u);
    }

    m_gauge01 = Clamp(g, 0.0f, 1.0f);
    m_gaugeJustMax = (prevGauge01 < 1.0f && m_gauge01 >= 1.0f);

    float curW = m_gaugeFinalW * m_gauge01;
    float curCenterX = m_gaugeLeftX + (curW * 0.5f);

    m_ribbonGauge.SetPos(curCenterX, m_gaugeY, 0);
    m_ribbonGauge.SetSize(curW, m_gaugeFinalH, 0);
    m_ribbonGauge.SetUVCrop(m_gauge01);

    if (!m_done)
    {
        if (m_loadInterval <= 1 || (m_frame % m_loadInterval) == 0)
        {
            m_done = tm.ProcessQueue(g_pDevice, m_loadPerFrame);
        }
    }

    if (!m_previewReady)
    {
        ID3D11ShaderResourceView* srv = tm.GetSRV(g_pDevice, "asset/Player/Player_Idle.png");
        if (srv)
        {
            srv->Release();
            m_previewPlayer.InitForLoadPreview();
            m_previewReady = true;
            m_previewPlayer.SetLoadPreviewAnim(m_done);
            m_wasDone = m_done;
        }
    }

    if (m_done && !m_wasDone && m_previewReady)
    {
        ID3D11ShaderResourceView* srvW = tm.GetSRV(g_pDevice, "asset/Player/Player_Walk.png");
        if (srvW) srvW->Release();

        m_previewPlayer.SetLoadPreviewAnim(true);
        m_wasDone = true;
    }

    if (m_previewReady)
    {
        m_previewPlayer.UpdateForLoadPreview(dt);
    }

    //if (m_done && m_gauge01 >= 1.0f)
    //{
    //    mgr.ChangeScene(m_next);
    //    g_sound->Play(SOUND_LABEL_SE_Ok);
    //    return;
    //}

   // if (m_mode == MODE_MINI)
    {
        MiniUpdate(dt);
    }
}

void SceneLoad::Draw()
{
    m_bg.Draw(g_pDeviceContext, g_pInputLayout, g_pVertexShader, g_pPixelShader, g_pConstantBuffer);

    if (m_previewReady)
    {
        m_previewPlayer.DrawForLoadPreview();
    }

    m_ribbonGauge.Draw(g_pDeviceContext, g_pInputLayout, g_pVertexShader, g_pPixelShader, g_pConstantBuffer);

   // if (m_done && m_mode == MODE_MINI)
    {
        MiniDraw();
    }
}

void SceneLoad::UnInit()
{
    m_bg.UnInit();
    m_ground.UnInit();
    m_playerBox.UnInit();
    for (size_t i = 0; i < m_obs.size(); ++i) m_obs[i].obj.UnInit();
    m_obs.clear();
    m_ribbonGauge.UnInit();
}

void SceneLoad::MiniInit()
{
    m_gameOver = false;
    m_score = 0.0f;

    m_groundY = -250.0f + m_miniOffsetY;

    m_ground.Init();
    m_ground.AddTexture("asset/Field/ブロック 5.png");
    m_ground.SetPos(0, -430.0f + m_miniOffsetY, 0);
    m_ground.SetSize(1920.0f, 80.0f * m_miniScale, 0);

    m_ground.SetUVMode(Object::UVMode::Loop);

    const float texW = 256.0f;
    m_ground.SetUVLength(1920.0f / texW);

    m_ground.SetUVScroll(0.0f);

    m_jumpDuration = 0.70f;
    m_jumpDelay = 0.08f;

    m_jumpY_Short = m_groundY + 230.0f * m_miniScale;
    m_jumpY_Long = m_groundY + 330.0f * m_miniScale;
    m_jumpY = m_jumpY_Short;

    m_playerBox.Init();
    m_playerBox.AddTexture("asset/Player/Player_Walk.png");
    m_playerY = m_groundY;
    m_playerBox.SetPos(-500, m_playerY, 0);
    m_playerBox.SetSize(90.0f * m_miniScale, 120.0f * m_miniScale, 0);

    m_jumping = false;
    m_jumpT = 0.0f;
    m_jumpDelayT = 0.0f;
    m_jumpHeld = false;
    m_jumpHoldT = 0.0f;

    for (size_t i = 0; i < m_obs.size(); ++i) m_obs[i].obj.UnInit();
    m_obs.clear();

    m_spawnTimer = 0.0f;
    m_spawnIntervalBase = 1.20f;
    m_spawnIntervalRand = 0.25f;
    m_spawnInterval = m_spawnIntervalBase + (RandRange(0, 100) / 100.0f) * m_spawnIntervalRand;

    m_speed = 850.0f;

    m_miniAnim = MINI_WALK;
    m_prevJumping = false;

    if (m_previewReady)
    {
        m_previewPlayer.SetPos(-500, m_playerY);
        m_previewPlayer.PlayWalkPreview();
    }

    m_lastType = ObstacleType::WingRippa;
    m_sameCount = 0;
    m_miniDamageLock = false;

    m_groundUV = 0.0f;
}

void SceneLoad::MiniReset()
{
    MiniInit();
}

bool SceneLoad::MiniCheckHitAABB(const Object& a, const Object& b)
{
    DirectX::XMFLOAT3 ap = a.GetPos();
    DirectX::XMFLOAT3 as = a.GetSize();
    DirectX::XMFLOAT3 bp = b.GetPos();
    DirectX::XMFLOAT3 bs = b.GetSize();

    float aL = ap.x - as.x * 0.5f;
    float aR = ap.x + as.x * 0.5f;
    float aB = ap.y - as.y * 0.5f;
    float aT = ap.y + as.y * 0.5f;

    float bL = bp.x - bs.x * 0.5f;
    float bR = bp.x + bs.x * 0.5f;
    float bB = bp.y - bs.y * 0.5f;
    float bT = bp.y + bs.y * 0.5f;

    if (aR < bL) return false;
    if (aL > bR) return false;
    if (aT < bB) return false;
    if (aB > bT) return false;
    return true;
}

SceneLoad::ObstacleType SceneLoad::PickRandomType()
{
    int r = RandRange(0, 99);
    ObstacleType t = ObstacleType::WingRippa;
    if (r < 40) t = ObstacleType::WingRippa;
    else if (r < 75) t = ObstacleType::Rippa;
    else t = ObstacleType::NeedleFloor;

    if (t == m_lastType)
    {
        m_sameCount++;
        if (m_sameCount >= 2)
        {
            t = (RandRange(0, 1) == 0) ? ObstacleType::WingRippa : ObstacleType::Rippa;
            m_sameCount = 0;
        }
    }
    else
    {
        m_sameCount = 0;
    }

    m_lastType = t;
    return t;
}

void SceneLoad::SpawnObstacle()
{
    Obstacle o;
    o.alive = true;
    o.obj.Init();

    const float xSpawn = 1100.0f;

    o.type = PickRandomType();

    if (o.type == ObstacleType::WingRippa)
    {
        o.obj.AddTexture("asset/Field/Wing_Rippa.png");
        o.obj.SetSize(350.0f * m_miniScale, 350.0f * m_miniScale, 0);
        o.obj.SetPos(xSpawn, m_groundY + 370.0f * m_miniScale, 0);
        o.requiredJumpY = 0.0f;
    }
    else if (o.type == ObstacleType::Rippa)
    {
        o.obj.AddTexture("asset/Field/rippa.png");
        o.obj.SetSize(300.0f * m_miniScale, 300.0f * m_miniScale, 0);
        o.obj.SetPos(xSpawn, m_groundY - 110.0f * m_miniScale, 0);
        o.requiredJumpY = m_jumpY_Short;
    }
    else
    {
        o.obj.AddTexture("asset/Field/needle_floor.png");
        o.obj.SetSize(600.0f * m_miniScale, 240.0f * m_miniScale, 0);
        o.obj.SetPos(xSpawn, m_groundY - 100.0f * m_miniScale, 0);
        o.requiredJumpY = m_jumpY_Long * 0.95f;
    }

    m_obs.push_back(o);

    float rnd = (RandRange(0, 100) / 100.0f);
    m_spawnInterval = m_spawnIntervalBase + rnd * m_spawnIntervalRand;

    m_speed += 3.0f;

    m_spawnIntervalBase = Clamp(m_spawnIntervalBase - 0.0020f, 0.55f, 1.60f);

    m_speed = Clamp(m_speed, 600.0f, 1900.0f);
}

void SceneLoad::MiniUpdate(float dt)
{
    bool jumpDown = input.GetKeyTrigger(VK_SPACE) || input.GetButtonTrigger(XINPUT_A);
    bool jumpHold = input.GetKeyPress(VK_SPACE) || input.GetButtonPress(XINPUT_A);
    bool jumpUp = (!jumpHold) && m_jumpHeld;
    m_jumpHeld = jumpHold;

    if (m_gameOver)
    {
        if (m_previewReady)
        {
            if (!m_miniDamageLock)
            {
                m_previewPlayer.PlayDamagePreview();
                m_miniDamageLock = true;
            }
            m_previewPlayer.UpdateForLoadPreview(dt);
        }

        if (input.GetKeyTrigger(VK_SPACE) || input.GetButtonTrigger(XINPUT_A))
        {
            MiniReset();
            return;
        }
        return;
    }

    const float texW = 256.0f;
    float du = (m_speed * dt) / texW;
    m_groundUV += du;

    if (m_groundUV > 1000.0f) m_groundUV -= 1000.0f;

    m_ground.SetUVScroll(m_groundUV);

    if (!m_jumping && jumpDown)
    {
        m_jumping = true;
        m_jumpT = 0.0f;
        m_jumpDelayT = 0.0f;
        m_jumpHoldT = 0.0f;
        m_jumpY = m_jumpY_Short;
    }

    if (m_jumping)
    {
        if (m_jumpDelayT < m_jumpDelay)
        {
            m_jumpDelayT += dt;
            m_playerY = m_groundY;
        }
        else
        {
            if (m_jumpT < 0.5f && jumpHold)
            {
                m_jumpHoldT += dt;
                if (m_jumpHoldT > m_jumpHoldMax) m_jumpHoldT = m_jumpHoldMax;
                float k = m_jumpHoldT / m_jumpHoldMax;
                m_jumpY = m_jumpY_Short + (m_jumpY_Long - m_jumpY_Short) * k;
            }
            if (jumpse == true)
            {
                g_sound->Play(SOUND_LABEL_SE_Jump);
                jumpse = false;
            }
            m_jumpT += dt / m_jumpDuration;
            float t01 = Clamp(m_jumpT, 0.0f, 1.0f);
            float updown = UpDown01(t01);
            m_playerY = m_groundY + (m_jumpY - m_groundY) * updown;

            if (m_jumpT >= 1.0f)
            {
                m_jumping = false;
                m_jumpT = 0.0f;
                m_jumpDelayT = 0.0f;
                m_jumpHoldT = 0.0f;
                m_playerY = m_groundY;
            }
        }
    }
    else
    {
        m_playerY = m_groundY;
        jumpse = true;
    }

    m_playerBox.SetPos(-500, m_playerY, 0);

    if (m_previewReady)
    {
        m_previewPlayer.SetPos(-500, m_playerY);

        MiniAnim want = MINI_WALK;
        if (m_gameOver) want = MINI_DAMAGE;
        else if (m_jumping) want = MINI_JUMP;

        if (want != m_miniAnim)
        {
            m_miniAnim = want;

            if (m_miniAnim == MINI_WALK)   m_previewPlayer.PlayWalkPreview();
            if (m_miniAnim == MINI_JUMP)   m_previewPlayer.PlayJumpPreview();
            if (m_miniAnim == MINI_DAMAGE) m_previewPlayer.PlayDamagePreview();
        }
    }

    m_spawnTimer += dt;
    if (m_spawnTimer >= m_spawnInterval)
    {
        m_spawnTimer = 0.0f;
        SpawnObstacle();
    }

    for (size_t i = 0; i < m_obs.size(); ++i)
    {
        if (!m_obs[i].alive) continue;

        DirectX::XMFLOAT3 p = m_obs[i].obj.GetPos();
        p.x -= m_speed * dt;
        m_obs[i].obj.SetPos(p.x, p.y, p.z);

        if (p.x < -1200)
        {
            m_obs[i].alive = false;
            continue;
        }

        if (MiniCheckHitAABB(m_playerBox, m_obs[i].obj))
        {
            m_gameOver = true;

            if (m_previewReady)
            {
                m_previewPlayer.PlayDamagePreview();
                g_sound->Play(SOUND_LABEL_SE_Damage);
            }
            break;
        }
        else
        {
            if (m_walkSETimer > 0.35f && walkchange == 0)
            {
                g_sound->Play(SOUND_LABEL_SE_MoveA);
                walkchange = 1;
                m_walkSETimer = 0.0f;
            }
            if (m_walkSETimer > 0.35f && walkchange == 1)
            {
                g_sound->Play(SOUND_LABEL_SE_MoveB);
                walkchange = 0;
                m_walkSETimer = 0.0f;
            }
        }
    }

    m_score += dt * 10.0f;
    m_previewPlayer.SetPos(-500, m_playerY);

}

void SceneLoad::MiniDraw()
{
    m_ground.Draw(g_pDeviceContext, g_pInputLayout, g_pVertexShader, g_pPixelShader, g_pConstantBuffer);

    for (size_t i = 0; i < m_obs.size(); ++i)
    {
        if (!m_obs[i].alive) continue;
        m_obs[i].obj.Draw(g_pDeviceContext, g_pInputLayout, g_pVertexShader, g_pPixelShader, g_pConstantBuffer);
    }
}

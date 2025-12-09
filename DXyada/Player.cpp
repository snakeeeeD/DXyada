#include "Player.h"
#include "Input.h"
#include "Renderer.h"
#include "Texture.h"
#include "Object.h"

extern Input input;
DirectX::XMFLOAT3 g_StartPlayer = { 0,0,0 };


void Player::Init() {

    m_player.Init("asset/char01.png", 3, 4);
    m_player.SetPos(g_StartPlayer.x, g_StartPlayer.y, 0);
    m_player.SetSize(100, 150, 0);

    // アニメーション設定
    m_player.AddAnimation({ "Down",0,0,2,3,true,1 });
    m_player.AddAnimation({ "Left",1,0,2,3,true,1 });
    m_player.AddAnimation({ "Right",2,0,2,3,true,1 });
    m_player.AddAnimation({ "Up",3,0,2,3,true,1 });
    m_player.AddAnimation({ "Idle",0,0,0,0,true,1 });

    // 重力・ジャンプ初期化
    m_velY = 0.0f;
    m_gravity = 2000.0f;    // 下方向加速度（ピクセル/秒^2）
    m_jumpPower = 600.0f;   // ジャンプ初速度
    m_isOnGround = true;

    deltaTime = 1.0f / 240.0f; // Update 毎に進む秒数
}

void Player::SetPos(float Pos_X,float Pos_Y) {
    m_player.SetPos(Pos_X , Pos_Y, 0);
}

void Player::Update(float deltaTime, const std::vector<Platform>& platforms, const std::vector<Enemy>& Enemy) {
    input.Update();

    auto pos = m_player.GetPos();

    if (input.GetKeyPress(VK_W)) { pos.y += 200.0f * deltaTime; m_player.PlayAnimation("Up"); }
    if (input.GetKeyPress(VK_S)) { pos.y -= 200.0f * deltaTime; m_player.PlayAnimation("Down"); }
    if (input.GetKeyPress(VK_A) || input.GetButtonPress(XINPUT_LEFT)) { pos.x -= 200.0f * deltaTime; m_player.PlayAnimation("Left"); }
    if (input.GetKeyPress(VK_D) || input.GetButtonPress(XINPUT_RIGHT)) { pos.x += 200.0f * deltaTime; m_player.PlayAnimation("Right"); }

    DirectX::XMFLOAT2 stick = input.GetLeftAnalogStick(); // -1～1

    const float threshold = 0.5f; // これだけ倒したら入力と判定

    if (stick.x > threshold) {
        pos.x += 200.0f * deltaTime;
        m_player.PlayAnimation("Right");
    }
    else if (stick.x < -threshold) {
        pos.x -= 200.0f * deltaTime;
        m_player.PlayAnimation("Left");
    }

    //ボタンを押していなかったらアイドルのアニメーション
    if (!(input.GetKeyPress(VK_W) || input.GetKeyPress(VK_S) || input.GetKeyPress(VK_A) || input.GetKeyPress(VK_D) 
        || input.GetButtonPress(XINPUT_LEFT) || input.GetButtonPress(XINPUT_RIGHT) ||
        fabs(input.GetLeftAnalogStick().x) > 0.5f))
        m_player.PlayAnimation("Idle");

    // 重力を加算
    m_velY += m_gravity * deltaTime;
    pos.y -= m_velY * deltaTime;

    // Platform上の着地判定
    m_isOnGround = false;
    for (auto& plat : platforms) {
        auto platPos = plat.GetObject()->GetPos();
        auto platSize = plat.GetObject()->GetSize();

        float platTop = platPos.y + platSize.y / 2.0f;
        float platLeft = platPos.x - platSize.x / 2.0f;
        float platRight = platPos.x + platSize.x / 2.0f;

        float playerBottom = pos.y - m_height / 2.0f;
        float playerLeft = pos.x - m_width / 2.0f;
        float playerRight = pos.x + m_width / 2.0f;

        // 横方向が重なるか
        if (playerRight > platLeft && playerLeft < platRight) {
            // 下端が上面に接触しているか
            if (playerBottom <= platTop && playerBottom >= platTop - 10.0f) {
                pos.y = platTop + m_height / 2.0f; // 補正
                m_velY = 0.0f;
                m_isOnGround = true;
            }
        }
    }

    // ジャンプ入力（地面にいる場合のみ）
    if (m_isOnGround && input.GetKeyTrigger(VK_SPACE) ||( m_isOnGround && input.GetButtonTrigger(XINPUT_A)))
    {
        m_velY = -m_jumpPower; // 上方向にジャンプ
        m_isOnGround = false;
    }

    // 最終的な位置セット
    m_player.SetPos(pos.x, pos.y, pos.z);

    // Objectのアニメーション更新
    m_player.Update(deltaTime);
}


void Player::Draw() {
    m_player.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
}

void Player::Uninit() {
    m_player.UnInit();
}

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

     m_guideline.Init("asset/block.png", 1, 1);
     m_guideline.SetPos(g_StartPlayer.x, g_StartPlayer.y, 0);
     m_guideline.SetSize(300, 20, 0);
}

void Player::SetPos(float Pos_X,float Pos_Y) {
    m_player.SetPos(Pos_X , Pos_Y, 0);
}

void Player::Update(float deltaTime, const std::vector<Platform>& platforms, const std::vector<Enemy>& Enemy) {
    input.Update();

    auto pos = m_player.GetPos();   //プレイヤーの位置を取得

    /*
  *  stick.x: 左(-1)〜右(1)
  *  stick.y: 下(-1)〜上(1)
  */
    DirectX::XMFLOAT2 stick = input.GetLeftAnalogStick(); // -1～1
    DirectX::XMFLOAT2 rightStick = input.GetRightAnalogStick();

    const float threshold = 0.5f; // これだけ倒したら入力と判定

    //キャラの移動処理
  
    //右入力があるか
    bool isMoveLeft = 
        input.GetKeyPress(VK_A) 
        || input.GetButtonPress(XINPUT_LEFT) 
        || (stick.x < -threshold);
        
    //左入力があるか
    bool isMoveRight =
        input.GetKeyPress(VK_D)
        || input.GetButtonPress(XINPUT_RIGHT)
        || (stick.x > threshold);

    //何も入力していない状態
    if (m_inputDir == 0)
    {
        //左入力があると
        if (isMoveLeft)
        {
            m_inputDir = 1;
        }
        //右入力があると
        else if (isMoveRight)
        {
            m_inputDir = -1;
        }
    }
    //左入力している場合
    else if (m_inputDir == 1)
    {
        //左入力を離す
        if (!isMoveLeft)
        {
            //右入力がされていたら
            if (isMoveRight)
            {
                m_inputDir = -1;    //右入力状態へ
            }
            //入力されていなかったら
            else
            {
                m_inputDir = 0;     //入力なし状態へ
            }
        }
    }
    //右入力している場合
    else if (m_inputDir == -1)
    {
        //右入力を離す
        if (!isMoveRight) 
        {
            //左入力がされていたら
            if (isMoveLeft)
            {
                m_inputDir = 1;     //左入力状態へ
            }
            else
            {
                m_inputDir = 0;     //入力なし状態へ
            }
        }
    }

    //左入力状態なら左移動
    if (m_inputDir == 1)
    {
        pos.x -= 200.0f * deltaTime; 
        m_player.PlayAnimation("Left");
    }

    //右入力状態なら右移動
   if (m_inputDir == -1)
    {
        pos.x += 200.0f * deltaTime;
        m_player.PlayAnimation("Right");
    }
  

    //ボタンを押していなかったらアイドルのアニメーション
   if (!(input.GetKeyPress(VK_W) || input.GetKeyPress(VK_S) || input.GetKeyPress(VK_A) || input.GetKeyPress(VK_D)
       || input.GetButtonPress(XINPUT_LEFT) || input.GetButtonPress(XINPUT_RIGHT) ||
       fabs(input.GetLeftAnalogStick().x) > 0.5f))
   {
       m_player.PlayAnimation("Idle");
   }

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

    float aimRightStick = 0.3;

    bool aiming = fabs(rightStick.x) > aimRightStick || fabs(rightStick.y) > aimRightStick;

        //右スティックを倒すと指示線表示
        if (aiming)
        {

            //ガイドライン表示
            m_guideline.SetColor(1, 1, 1, 0.5);

            //プレイヤーの位置取得
            auto p = m_player.GetPos();

            //指示線の位置をプレイヤーの中心へ
            m_guideline.SetPos(p.x, p.y, p.z);


            //角度を右スティック方向に合わせる
            float angleRad = atan2(rightStick.y, rightStick.x);

            m_guideline.SetPos(p.x + 150, p.y, p.z);

            //ラジアンを度へ変換
            float angleDeg = angleRad * (180.0f / DirectX::XM_PI);

            //回転の中心を左端に移動
            m_guideline.SetPivot(-150, 0, 0);

            //角度を設定
            m_guideline.SetAngle(angleDeg);

            //指示線を更新
            m_guideline.Update(deltaTime);

        }
        else
        {
            //透明に
            m_guideline.SetColor(1, 1, 1, 0);
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

    m_guideline.Draw(
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );

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
    m_guideline.UnInit();
}


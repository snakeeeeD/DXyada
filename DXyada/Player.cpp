#include "Player.h"
#include "Input.h"
#include "Renderer.h"
#include "Texture.h"
#include "Object.h"
#include "framework.h"


extern Input input;
DirectX::XMFLOAT3 g_StartPlayer = { 0,0,0 };
enum AnimState
{
Idle,
Run, 
Jump,
Damage,
Libbon
};

void Player::Init() {

    m_player.Init();
    m_player.SetPos(g_StartPlayer.x, g_StartPlayer.y, 0);
    m_player.SetSize(100, m_height, 0);

    // アニメーション設定
    m_player.AddAnimation("Left", "asset/Player_Work.png", 4, 1, 0, 0, 1, 5, true, 1);
    m_player.AddAnimation("Right", "asset/Player_Work.png", 4, 1, 0, 2, 3, 5, true, 1);
    m_player.AddAnimation("LeftIdle", "asset/Player_Idle.png",  3, 2, 0, 0, 2, 5, true, 1);
    m_player.AddAnimation("RightIdle", "asset/Player_Idle.png", 3, 2, 1, 0, 2, 5, true, 1);
    m_player.AddAnimation("LJump", "asset/Player_SmallJump.png", 5, 2, 0, 0, 4, 9, false, 2);
    m_player.AddAnimation("RJump", "asset/Player_SmallJump.png", 5, 2, 1, 0, 4, 9, false, 2);
    m_player.AddAnimation("LDamage", "asset/Player_Damage.png", 5, 2, 0, 0, 4, 9, false, 3);
    m_player.AddAnimation("RDamage", "asset/Player_Damage.png", 5, 2, 1, 0, 4, 9, false, 3);
    m_player.AddAnimation("LOutLibbon", "asset/Player_Ribbon.png", 5, 2, 0, 0, 4, 9, false, 2);
    m_player.AddAnimation("ROutLibbon", "asset/Player_Ribbon.png", 5, 2, 1, 0, 4, 9, false, 2);

    // 重力・ジャンプ初期化
    m_velY = 0.0f;
    m_gravity = 2000.0f;    // 下方向加速度（ピクセル/秒^2）
    m_jumpPower = 600.0f;   // ジャンプ初速度
    m_isOnGround = true;

    //HP初期化
    m_maxhp = 3;    //仮
    m_hp = m_maxhp;
    m_isInvincible = false;
    m_Invincibletimer = 0.0f;
    m_InvincibleDuration = 2.0f;    //二秒間無敵

    //ノックバック初期化
    m_isKnockback = false;
    m_knockbackTimer = 0.0f;
    m_knockbackDuration = 0.6f;  // 0.6秒間ノックバック
    m_knockbackVelocity = { 0.0f, 0.0f };

    //指示線用
    m_baseGuidelineLength = m_height * 3.5;//身長の3.5倍の長さ
    m_guideline.Init();
    m_guideline.AddTexture("asset/block.png");
    m_guideline.SetPos(g_StartPlayer.x, g_StartPlayer.y, 0);
    m_guideline.SetSize(m_baseGuidelineLength, 20, 0);  

    m_Circle.Init();
    m_Circle.AddTexture("asset/circle.png");
    m_Circle.SetSize(100, 100, 0);

    m_ribbon.Init();
}

void Player::TakeDamage(int damage, DirectX::XMFLOAT2 knockbackDir)
{
    if (m_isInvincible || m_hp <= 0) return;

    m_hp -= damage;
    if (m_hp < 0) m_hp = 0;

    //ダメージを受けたら無敵時間開始
    m_isInvincible = true;
    m_Invincibletimer = 0.0f;

    //ノックバック開始
    m_isKnockback = true;
    m_knockbackTimer = 0.0f;

    //ノックバック方向を初期化して速度を設定
    float length = sqrt(knockbackDir.x * knockbackDir.x + knockbackDir.y * knockbackDir.y);
    if (length > 0.0f)
    {
        m_knockbackVelocity.x = (knockbackDir.x / length) * 400.0f;  // ノックバック速度
        m_knockbackVelocity.y = (knockbackDir.y / length) * 400.0f;  // 上方向も少し
    }

    // 上方向に最低限のノックバック速度を保証
    if (m_knockbackVelocity.y > -200.0f)
    {
        m_knockbackVelocity.y = -400.0f;
    }
}

void Player::SetPos(float Pos_X, float Pos_Y) {
    m_player.SetPos(Pos_X, Pos_Y, 0);
}

void Player::Update(float deltaTime, const std::vector<Platform>& platforms, const std::vector<Enemy*>& Enemy) {

    //Hpが0以下なら更新しない
    if (m_hp <= 0)
    {

        return;
    }

    //無敵時間の更新
    if (m_isInvincible)
    {
        m_Invincibletimer += deltaTime;
        if (m_Invincibletimer >= m_InvincibleDuration)
        {
            m_isInvincible = false;
            m_Invincibletimer = 0.0f;
        }

        //無敵時間中を点滅
        float blincspeed = 10.0f;
        float alpha = (sin(m_Invincibletimer * blincspeed) + 1.0f) * 0.5f;
        m_player.SetColor(1, 1, 1, alpha * 0.5f + 0.5f);
    }
    else
    {
        m_player.SetColor(1, 1, 1, 1);
    }

    //ノックバック更新
    if (m_isKnockback)
    {
        if (m_knockbackVelocity.x > 0)
        {
            m_player.PlayAnimation("LDamage");
        }
        else if (m_knockbackVelocity.x <= 0)
        {
            m_player.PlayAnimation("RDamage");
        }

        m_knockbackTimer += deltaTime;
        if (m_knockbackTimer >= m_knockbackDuration)
        {
            m_isKnockback = false;
            m_knockbackTimer = 0.0f;
            m_knockbackVelocity = { 0.0f, 0.0f };
        }
    }

    auto pos = m_player.GetPos();   //プレイヤーの位置を取得

    /*
  *  stick.x: 左(-1)〜右(1)
  *  stick.y: 下(-1)〜上(1)
  */
    DirectX::XMFLOAT2 stick = input.GetLeftAnalogStick(); // -1～1
    DirectX::XMFLOAT2 rightStick = input.GetRightAnalogStick();

    const float threshold = 0.5f; // これだけ倒したら入力と判定

    // 重力を加算
    m_velY += m_gravity * deltaTime;
    pos.y -= m_velY * deltaTime;

    // Platform上の着地判定
    m_isOnGround = false;
    for (auto& plat : platforms) 
    {
        auto platPos = plat.GetObject()->GetPos();
        auto platSize = plat.GetObject()->GetSize();

        float platTop = platPos.y + platSize.y / 2.0f;
        float platLeft = platPos.x - platSize.x / 2.0f;
        float platRight = platPos.x + platSize.x / 2.0f;

        float playerBottom = pos.y - m_height / 2.0f;
        float playerLeft = pos.x - m_width / 2.0f;
        float playerRight = pos.x + m_width / 2.0f;

        // 横方向が重なるか
        if (playerRight > platLeft && playerLeft < platRight) 
        {
            // 下端が上面に接触しているか
            if (playerBottom <= platTop && playerBottom >= platTop - 10.0f) 
            {
                pos.y = platTop + m_height / 2.0f; // 補正
                m_velY = 0.0f;
                m_isOnGround = true;
            }
        }
    }

    //キャラの移動処理

    //ノックバック中出ない場合のみ移動
    if (!m_isKnockback)
    {


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
                m_isLastRightDirection = false;
            }
            //右入力があると
            else if (isMoveRight)
            {
                m_inputDir = -1;
                m_isLastRightDirection = true;

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
                    m_isLastRightDirection = true;
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
                    m_isLastRightDirection = false;
                }
                else
                {
                    m_inputDir = 0;     //入力なし状態へ
                }
            }
        }
        // ジャンプ入力（地面にいる場合のみ）
        if (m_isOnGround && (input.GetKeyTrigger(VK_SPACE) || input.GetButtonTrigger(XINPUT_A))) 
        {
            m_velY = -m_jumpPower; // 上方向にジャンプ
            m_isOnGround = false;
        }
        if (!m_isOnGround) 
        {
            if (m_inputDir == 1)
            {
                pos.x -= 200.0f * deltaTime;
                m_player.PlayAnimation("LJump");
            }
            //右入力状態なら右移動
            else if (m_inputDir == -1)
            {
                pos.x += 200.0f * deltaTime;
                m_player.PlayAnimation("RJump");
            }
            else
            {
                if (m_isLastRightDirection)
                {
                    m_player.PlayAnimation("RJump");
                }
                else if (!m_isLastRightDirection)
                {
                    m_player.PlayAnimation("LJump");
                }
            }
        }
        else 
        {

            //ボタンを押していなかったらアイドルのアニメーション
            if (!(input.GetKeyPress(VK_W) || input.GetKeyPress(VK_S) || input.GetKeyPress(VK_A) || input.GetKeyPress(VK_D)
                || input.GetButtonPress(XINPUT_LEFT) || input.GetButtonPress(XINPUT_RIGHT) ||
                fabs(input.GetLeftAnalogStick().x) > 0.5f))
            {
                if (m_ribbon.GetState() == Ribbon::State::Idle)
                {
                    if (m_isLastRightDirection)
                    {
                        m_player.PlayAnimation("RightIdle");
                    }
                    else if (!m_isLastRightDirection)
                    {
                        m_player.PlayAnimation("LeftIdle");
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

        }

    }
    else
    {
        //ノックバック中は強制移動
        pos.x += m_knockbackVelocity.x * deltaTime;
        pos.y -= m_knockbackVelocity.y * deltaTime;

        // ノックバック速度を減衰
        m_knockbackVelocity.x *= 0.95f;
        m_knockbackVelocity.y *= 0.95f;
    }


    float aimRightStick = 0.3;

    bool aiming = fabs(rightStick.x) > aimRightStick || fabs(rightStick.y) > aimRightStick;

    //右スティックを倒すと指示線表示
    if (aiming)
    {
        //プレイヤーの位置取得
        auto p = m_player.GetPos();

        //角度を右スティック方向に合わせる
        float angleRad = atan2(rightStick.y, rightStick.x);
        float GuideangleRad = atan2(rightStick.y, rightStick.x);

        float dirX = cos(angleRad);
        float dirY = sin(angleRad);

        float guidelineLength = m_baseGuidelineLength;
        float minPerpendicularDist = FLT_MAX;   //垂直距離の最小値
        bool foundEnemy = false;

        //+-20度の弧で範囲チェック
        float angleThreshold = cos(20.0f * DirectX::XM_PI / 180.0f);

        for (const auto& enemy : Enemy)
        {
            auto enemyPos = enemy->GetObject()->GetPos();

            float toenemyX = enemyPos.x - p.x;
            float toenemyY = enemyPos.y - p.y;
            float distToEnemy = sqrt(toenemyX * toenemyX + toenemyY * toenemyY);

            if (distToEnemy > m_baseGuidelineLength) continue;

            float dotProduct = (dirX * toenemyX + dirY * toenemyY) / distToEnemy;

            if (dotProduct > angleThreshold)
            {
                //指示線からの垂直距離を計算
                float projectionLength = dotProduct * distToEnemy;  //指示線の投影距離
                float perpendicularDist = sqrt(distToEnemy * distToEnemy - projectionLength * projectionLength);

                //指示線から一番近い敵を選択
                if (perpendicularDist < minPerpendicularDist)
                {
                    minPerpendicularDist = perpendicularDist;
                    guidelineLength = projectionLength;
                    foundEnemy = true;
                    m_targetPosition = enemyPos;
                }
            }
        }

        m_hasTarget = foundEnemy;
        if (foundEnemy)
        {

            // プレイヤーから敵への方向を再計算
            float toTargetX = m_targetPosition.x - p.x;
            float toTargetY = m_targetPosition.y - p.y;
            angleRad = atan2(toTargetY, toTargetX);
            dirX = cos(angleRad);
            dirY = sin(angleRad);

            //ガイドライン表示
            m_guideline.SetColor(1, 0.5, 0.5, 0.8);

            m_Circle.SetPos(m_targetPosition.x, m_targetPosition.y, 0);
            m_Circle.SetColor(1, 1, 1, 1);
        }
        else
        {
            guidelineLength = m_baseGuidelineLength;
            //ガイドライン表示
            m_guideline.SetColor(1, 1, 1, 0.5);

            m_Circle.SetColor(1, 1, 1, 0);
        }

        m_aimDirection = { dirX, dirY };

        if (m_ribbon.GetState() != Ribbon::State::Throwing)
        {
            float centerX = p.x + (guidelineLength / 2.0f) * dirX;
            float centerY = p.y + (guidelineLength / 2.0f) * dirY;

            //プレイヤーの中心に指示線の左端が来るように
            m_guideline.SetPos(p.x + (m_baseGuidelineLength / 2), p.y, p.z);

            //ラジアンを度へ変換
            float angleDeg = angleRad * (180.0f / DirectX::XM_PI);
            float GuideangleDeg = GuideangleRad * (180.0f / DirectX::XM_PI);

            //回転の中心を左端に移動
            m_guideline.SetPivot(((-m_baseGuidelineLength) / 2), 0, 0);

            //角度を設定
            m_guideline.SetAngle(GuideangleDeg);

            //指示線を更新
            m_guideline.Update(deltaTime);
        }
        else
        {
             //透明に
            m_guideline.SetColor(1, 1, 1, 0);
        }
       
            if (rightStick.x > 0)
            {
                m_isLastRightDirection = true;
            }
            else if (rightStick.x < 0)
            {
                m_isLastRightDirection = false;
            }
    
    }
    else
    {
        //透明に
        m_guideline.SetColor(1, 1, 1, 0);
        m_Circle.SetColor(1, 1, 1, 0);
    }

    // 毎フレーム
    m_ribbon.SetPlayerPos({ pos.x, pos.y });
    m_ribbon.Update(deltaTime);

    float rightTrigger = input.GetRightTrigger();
    bool isRTPressed = rightTrigger > 0.5f;

    if (!m_isKnockback)
    {
        // キー入力
        if ((input.GetKeyTrigger(VK_X) || (isRTPressed && !m_wasRTPressed)) && !m_isRibbonOut)
        {
            //右スティックか倒されてるか確認
            if (aiming)
            {
                //右スティックの状態を正規化して送る

                m_ribbon.Throw(m_aimDirection);
                if (rightStick.x > 0)
                {
                    m_player.PlayAnimation("ROutLibbon");
                }
                else if (rightStick.x < 0)
                {
                    m_player.PlayAnimation("LOutLibbon");
                }
              
            }
            else
            {
                // 右スティックが倒されていない場合は最後に向いた方向
                if (m_isLastRightDirection)
                {
                    m_ribbon.Throw({ 1.0f, 0.0f }); // 右方向
                    m_player.PlayAnimation("ROutLibbon");
                }
                else
                {
                    m_ribbon.Throw({ -1.0f, 0.0f }); // 左方向
                    m_player.PlayAnimation("LOutLibbon");
                }
            }
            m_isRibbonOut = true;

        }
        //キーボードでも複数回リボンを伸ばせるように修正
        if ((m_ribbon.GetState() == Ribbon::State::Returning) || (!isRTPressed && m_wasRTPressed))
        {
            if (m_isRibbonOut)
            {
                m_ribbon.Return();
                m_isRibbonOut = false;
            }
        }
    }
    

    //次フレームのために現在の状態を保持
    m_wasRTPressed = isRTPressed;

    // 最終的な位置セット
    m_player.SetPos(pos.x, pos.y, pos.z);

    // Objectのアニメーション更新
    m_player.Update(deltaTime);
}



void Player::Draw() 
{
    m_player.Draw
    (
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );

    m_Circle.Draw
    (
        g_pDeviceContext,
        g_pInputLayout,
        g_pVertexShader,
        g_pPixelShader,
        g_pConstantBuffer
    );
}

void Player::Uninit() 
{
    m_player.UnInit();
    m_guideline.UnInit();
    m_ribbon.UnInit();
    m_Circle.UnInit();
}

// Ribbon 取得
Ribbon& Player::GetRibbon()
{
    return m_ribbon;
}

bool Player::IsEnemyInRange(const DirectX::XMFLOAT3& enemyPos, float& distance) const
{
    auto playerpos = m_player.GetPos();

    float dx = enemyPos.x - playerpos.x;
    float dy = enemyPos.y - playerpos.y;

    //四角の範囲チェック
    if (fabs(dx) > m_detectionRangeSquare || fabs(dy) > m_detectionRangeSquare)
    {
        return false;
    }

    //円の範囲チェック
    distance = sqrt(dx * dx + dy * dy);
    if (distance > m_detectionRangeCircle)
    {
        return false;
    }

    return true;
}
#include "Player.h"
#include "Input.h"
#include "Renderer.h"
#include "Texture.h"
#include "Object.h"
#include "framework.h"
#include "CollisionManager.h"


extern Input input;
DirectX::XMFLOAT3 g_StartPlayer = { 0,0,0 };

static bool ResolveGroundLanding(
    DirectX::XMFLOAT3& ioPos,      // プレイヤー位置（更新される）
    float& ioVelY,                 // 落下速度（更新される）
    bool& ioIsOnGround,            // 接地フラグ（更新される）
    float playerW,
    float playerH,
    const DirectX::XMFLOAT3& groundPos,
    const DirectX::XMFLOAT3& groundSize
)
{
    // ground は中心座標 + 幅高さ（あなたの Object/Platform と同じ前提）
    float platTop = groundPos.y + groundSize.y * 0.5f;
    float platLeft = groundPos.x - groundSize.x * 0.5f;
    float platRight = groundPos.x + groundSize.x * 0.5f;

    float playerBottom = ioPos.y - playerH * 0.5f;
    float playerLeft = ioPos.x - playerW * 0.5f;
    float playerRight = ioPos.x + playerW * 0.5f;

    // X が重なっているか
    if (playerRight > platLeft && playerLeft < platRight)
    {
        // 「上から着地」だけ拾う（50は許容）
        if (playerBottom <= platTop && playerBottom >= platTop - 50.0f)
        {
            ioPos.y = platTop + playerH * 0.5f;
            ioVelY = 0.0f;
            ioIsOnGround = true;
            return true;
        }
    }
    return false;
}

void Player::Init() {

    m_player.Init();
    m_player.SetPos(g_StartPlayer.x, g_StartPlayer.y, 0);
    m_player.SetSize(100, m_height, 0);

    // アニメーション設定
    //アイドル//
    m_player.AddAnimation("LeftIdle", "asset/Player/Player_Idle.png", 4, 2, 1, 0, 3, 5, true, 1);
    m_player.AddAnimation("RightIdle", "asset/Player/Player_Idle.png", 4, 2, 0, 0, 3, 5, true, 1);

    //移動//
    m_player.AddAnimation("Left", "asset/Player/Player_Walk.png", 4, 2, 1, 0, 3, 10, true, 1);
    m_player.AddAnimation("Right", "asset/Player/Player_Walk.png", 4, 2, 0, 0, 3, 10, true, 1);

    //小ジャンプ//
    m_player.AddAnimation("LJump", "asset/Player/Player_SmallJump.png", 5, 2, 1, 0, 4, 9, false, 2);
    m_player.AddAnimation("RJump", "asset/Player/Player_SmallJump.png", 5, 2, 0, 0, 4, 9, false, 2);

    //ダメージ//
    m_player.AddAnimation("LDamage", "asset/Player/Player_Damage.png", 5, 2, 1, 0, 4, 9, false, 3);
    m_player.AddAnimation("RDamage", "asset/Player/Player_Damage.png", 5, 2, 0, 0, 4, 9, false, 3);

    //リボン出し//
    m_player.AddAnimation("LOutLibbon", "asset/Player/Player_Ribbon.png", 5, 2, 1, 0, 4, 15, false, 2);
    m_player.AddAnimation("ROutLibbon", "asset/Player/Player_Ribbon.png", 5, 2, 0, 0, 4, 15, false, 2);

    //リボンジャンプ//
    m_player.AddAnimation("LRibbonJump", "asset/Player/Player_RibbonJump.png", 5, 2, 1, 0, 4, 9, false, 2);
    m_player.AddAnimation("RRibbonJump", "asset/Player/Player_RibbonJump.png", 5, 2, 0, 0, 4, 9, false, 2);

    //引き寄せられる//
    m_player.AddAnimation("LPulled", "asset/Player/Player_Pulled.png", 5, 2, 1, 0, 4, 15, true, 2);
    m_player.AddAnimation("RPulled", "asset/Player/Player_Pulled.png", 5, 2, 0, 0, 4, 15, true, 2);

    //引き寄せる
    m_player.AddAnimation("LRoll", "asset/Player/Player_Roll.png", 5, 2, 1, 0, 4, 15, true, 2);
    m_player.AddAnimation("RRoll", "asset/Player/Player_Roll.png", 5, 2, 0, 0, 4, 15, true, 2);
    m_animState = Idle;
    m_prevAnimState = static_cast<Player_AnimState>(-1);

    m_prevIsRightDirection = m_isLastRightDirection;
    m_isThrowingThisFrame = false;
    m_waitReleaseAfterThrow = false;

    m_isCanMove = true;

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
    m_guideline.AddTexture("asset/Field/block.png");
    m_guideline.SetPos(g_StartPlayer.x, g_StartPlayer.y, 0);
    m_guideline.SetSize(m_baseGuidelineLength, 20, 0);

    m_Circle.Init();
    m_Circle.AddTexture("asset/Player/Circle2.png");
    m_Circle.SetSize(100, 100, 0);

    m_ribbon.Init();
    m_ribbon.SetCollisionManager(m_collisionMgr);

    //RT長押し撃破初期化
    m_holdRTTimer = 0.0f;
    m_holdRTRequired = 3.0f;  // 3秒長押しで離す

    //LT長押し撃破初期化
    m_holdLTTimer = 0.0f;
    m_holdLTRequired = 0.5f;  // 1秒長押しで撃破
    m_targetEnemy = nullptr;


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

void Player::Update(
    float deltaTime,
    const std::vector<Platform>& platforms,
    std::vector<Enemy*>& enemies,
    std::vector<Pin*>& pins
)
{
    auto pos = m_player.GetPos();
    float prevY = pos.y;

    //Hpが0以下なら更新しない
    if (m_hp <= 0)
        return;

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
        float blink = (sinf(m_Invincibletimer * 10.0f) + 1.0f) * 0.5f;
        m_player.SetColor(1, 1, 1, blink * 0.5f + 0.5f);
    }
    else
    {
        m_player.SetColor(1, 1, 1, 1);
    }

    if (m_collisionMgr)
    {
        m_collisionMgr->SetPlayerInvincible(m_isInvincible);
    }

    //ノックバック更新
    if (m_isKnockback)
    {
        m_knockbackTimer += deltaTime;
        if (m_knockbackTimer >= m_knockbackDuration)
        {
            m_isKnockback = false;
            m_knockbackTimer = 0.0f;
            m_knockbackVelocity = { 0,0 };
        }
    }

    float rightTrigger = input.GetRightTrigger();
    float leftTrigger = input.GetLeftTrigger();
    bool isRTPressed = rightTrigger > 0.5f;
    bool isLTPressed = leftTrigger > 0.5f;

    bool wasPinGrabbedLastFrame = (m_ribbon.GetHitPin() != nullptr && m_wasRTPressed);

    //========================================
    // リボン先端ヒット判定
    //========================================

    // RibbonにRTの状態を伝える
    m_ribbon.SetRTheld(isRTPressed);

    Enemy* hitEnemy = m_ribbon.GetHitEnemy();
    Pin* hitPin = m_ribbon.GetHitPin();
    // BlockPinかどうかをチェック
    BlockPin* blockPin = dynamic_cast<BlockPin*>(hitPin);
    // RemoteWindPinかどうかをチェック
    RemoteWindPin* remotewindPin = dynamic_cast<RemoteWindPin*>(hitPin);

    //--------------------------------------
    // エイム入力（右スティック or マウス）
    //--------------------------------------
    DirectX::XMFLOAT2 leftStick = input.GetLeftAnalogStick();// 移動専用
    DirectX::XMFLOAT2 rightStick = input.GetRightAnalogStick();// エイム専用
    const float moveThresholdR = 0.1f;  //右スティックのデッドゾーン(あえて甘く判定とってます)
    const float moveThresholdL = 0.5f;  //左スティックのデッドゾーン

    //左スティックの今フレームの上入力
    bool stickUpNow = (leftStick.y > moveThresholdL);

    //==========================================================
    // 
    // 左スティックの角度(30°～150°)ばーじょん
    // 
    // 
    //float angle = atan2(leftStick.y, leftStick.x); // -π ～ π

    //float length = sqrt(leftStick.x * leftStick.x + leftStick.y * leftStick.y);

    //bool stickUpNow = false;

    //if (length > 0.3)
    //{
    //    float angle = atan2(leftStick.y, leftStick.x);

    //    stickUpNow =
    //        angle > DirectX::XM_PI * 30.0f / 180.0f &&   // 30°
    //        angle < DirectX::XM_PI * 150.0f / 180.0f;    // 150°
    //}
    //==========================================================
  
    //左スティック立ち上がり判定
    bool leftStickUpTrigger = stickUpNow && !m_prevLeftStickUp;

    // 右スティック判定
    bool stickAiming =
        fabs(rightStick.x) > moveThresholdR ||
        fabs(rightStick.y) > moveThresholdR;

    // マウス判定
    POINT mouse = input.GetMousePos();

    DirectX::XMFLOAT2 toMouse;
    toMouse.x = mouse.x - pos.x;
    toMouse.y = -(mouse.y - pos.y);

    float mouseLen = sqrt(toMouse.x * toMouse.x + toMouse.y * toMouse.y);
    bool mouseAiming = mouseLen > 5.0f;
    bool mousePressed = input.GetMouseButtonPress(VK_LBUTTON);
    // エイム方向を1本化
    bool aiming = false;

    bool foundEnemy = false;
    bool foundPin = false;



    if (stickAiming)
    {
        m_aimDirection = rightStick;
        aiming = true;
    }
    else if (mouseAiming && mousePressed)
    {
        m_aimDirection.x = toMouse.x / mouseLen;
        m_aimDirection.y = toMouse.y / mouseLen;
        aiming = true;
    }
    else
    {
        m_guideline.SetColor(1, 1, 1, 0);
        m_Circle.SetColor(1, 1, 1, 0);
    }

    //右スティック一回転検出（つかんでいる場合）
    //(仮で敵に引っかかったとき)
    if ((hitEnemy || hitPin) && isRTPressed && stickAiming)
    {
        // 現在の角度を計算
        float currentAngle = atan2(rightStick.y, rightStick.x);

        if (m_isRotating)
        {
            //現在の差分を計算
            float angleDiff = currentAngle - m_prevStickAngle;

            //-πを超えた場合の補正
            if (angleDiff > DirectX::XM_PI)
            {
                angleDiff -= DirectX::XM_2PI;
            }
            else if (angleDiff < -DirectX::XM_PI)
            {
                angleDiff += DirectX::XM_2PI;
            }

            m_totalRotation += angleDiff;

            //回転に応じて引き寄せ
            if (fabs(m_totalRotation) > 0.1f)
            {
                if (!hitEnemy)
                {
                    m_isPulling = true;
                }
            }
        }
        else
        {
            //回転開始
            m_isRotating = true;
            m_totalRotation = 0.0f;
        }

        m_prevStickAngle = currentAngle;
    }
    else
    {
        //スティックを離すとリセット
        m_isRotating = false;
        m_isPulling = false;
    }


    // 重力を加算
    m_velY += m_gravity * deltaTime;
    pos.y -= m_velY * deltaTime;

    m_isOnGround = false;

    //// Platform上の着地判定
    //for (auto& plat : platforms)
    //{
    //    auto pPos = plat.GetObject()->GetPos();
    //    auto pSize = plat.GetObject()->GetSize();

    //    float platTop = pPos.y + pSize.y * 0.5f;
    //    float platLeft = pPos.x - pSize.x * 0.5f;
    //    float platRight = pPos.x + pSize.x * 0.5f;

    //    float playerBottom = pos.y - m_height * 0.5f;
    //    float playerLeft = pos.x - m_width * 0.5f;
    //    float playerRight = pos.x + m_width * 0.5f;

    //    if (playerRight > platLeft && playerLeft < platRight)
    //    {
    //        if (playerBottom <= platTop && playerBottom >= platTop - 50.0f)
    //        {
    //            pos.y = platTop + m_height * 0.5f;
    //            m_velY = 0.0f;
    //            m_isOnGround = true;
    //        }
    //    }
    //}

    //// 飾ったピンを地面扱い
    //for (auto* pin : pins)
    //{
    //    if (!pin) continue;

    //    // ここで「地面になれるPin」だけ許可
    //    if (!pin->CanBeGround())
    //        continue;

    //    // ★ Decorated限定をやめて、Pin側の判定に寄せる
    //    if (!pin->IsGround())
    //        continue;

    //    auto pPos = pin->GetObject()->GetPos();
    //    auto pSize = pin->GetObject()->GetSize();

    //    float platTop = pPos.y + pSize.y * 0.5f;
    //    float platLeft = pPos.x - pSize.x * 0.5f;
    //    float platRight = pPos.x + pSize.x * 0.5f;

    //    float playerBottom = pos.y - m_height * 0.5f;
    //    float playerLeft = pos.x - m_width * 0.5f;
    //    float playerRight = pos.x + m_width * 0.5f;

    //    if (playerRight > platLeft && playerLeft < platRight)
    //    {
    //        if (playerBottom <= platTop && playerBottom >= platTop - 50.0f)
    //        {
    //            pos.y = platTop + m_height * 0.5f;
    //            m_velY = 0.0f;
    //            m_isOnGround = true;
    //        }
    //    }
    //}

// 地面（Platformタグ）に対する着地判定
    if (m_collisionMgr)
    {
        const std::vector<Object*>& grounds =
            m_collisionMgr->GetObjectsByTag(ColliderTag::Platform);

        for (size_t i = 0; i < grounds.size(); ++i)
        {
            Object* ground = grounds[i];
            if (!ground) continue;

            DirectX::XMFLOAT3 pPos = ground->GetPos();
            DirectX::XMFLOAT3 pSize = ground->GetSize();

            float platTop = pPos.y + pSize.y * 0.5f;
            float platLeft = pPos.x - pSize.x * 0.5f;
            float platRight = pPos.x + pSize.x * 0.5f;

            float playerBottom = pos.y - m_height * 0.5f;
            float playerLeft = pos.x - m_width * 0.5f;
            float playerRight = pos.x + m_width * 0.5f;

            if (playerRight > platLeft && playerLeft < platRight)
            {
                if (playerBottom <= platTop && playerBottom >= platTop - 50.0f)
                {
                    pos.y = platTop + m_height * 0.5f;
                    m_velY = 0.0f;
                    m_isOnGround = true;
                }
            }
        }
    }


    if (m_isCanMove == true)
    {
        //操作関係
        m_moveLeft =
            input.GetKeyPress(VK_A) ||
            input.GetButtonPress(XINPUT_LEFT) ||
            (leftStick.x < -moveThresholdL);

        m_moveRight =
            input.GetKeyPress(VK_D) ||
            input.GetButtonPress(XINPUT_RIGHT) ||
            (leftStick.x > moveThresholdL);
    }

    if (m_isPinJumping)
    {
        pos.x += m_pinJumpVelocity.x * deltaTime;

        // 速度を減衰
        m_pinJumpVelocity.x *= 0.98f;

        // 速度が十分小さくなったらジャンプ終了
        if (m_isOnGround)
        {
            m_isPinJumping = false;
            m_pinJumpVelocity = { 0.0f, 0.0f };
        }
    }
    //ノックバック中出ない場合のみ移動
    else if (!m_isKnockback)
    {
        if (m_moveLeft && !m_moveRight)       m_inputDir = 1;
        else if (m_moveRight && !m_moveLeft)  m_inputDir = -1;
        else                              m_inputDir = 0;

        // 通常の移動
        if (m_inputDir == 1)  pos.x -= 350.0f * deltaTime;
        if (m_inputDir == -1) pos.x += 350.0f * deltaTime;

        // 敵を掴んでいる場合の処理
        if (hitEnemy)
        {
            auto enemyPos = hitEnemy->GetObject()->GetPos();
            float dx = enemyPos.x - pos.x;
            float dy = enemyPos.y - pos.y;
            float currentDist = sqrt(dx * dx + dy * dy);

            if (isRTPressed)
            {

                if (m_isPulling)
                {
                    // 最大長を超えたら自動でリボンを外す
                    if (currentDist > m_baseGuidelineLength)
                    {
                        m_ribbon.Return();
                        m_isRibbonOut = false;
                        hitEnemy->SetSlow(false);
                        m_totalRotation = 0.0f;
                        m_isPulling = false;
                        m_isRotating = false;
                    }
                }
                else if (!hitEnemy)
                {
                    // 敵を掴んでいない場合はリセット
                    m_totalRotation = 0.0f;
                    m_isPulling = false;
                    m_isRotating = false;
                }


            }
            else
            {
                m_ribbon.Return();
            }

            // LT長押しで敵を撃破
            if (isLTPressed)
            {
                //Enemyを停止
                hitEnemy->SetSlow(true);

                // 対象の敵が変わったらタイマーリセット
                if (m_targetEnemy != hitEnemy)
                {
                    m_targetEnemy = hitEnemy;
                    m_holdLTTimer = 0.0f;
                }

                // タイマー加算
                m_holdLTTimer += deltaTime;

                if ((m_holdLTTimer >= m_holdLTRequired))
                {
                    m_justDeco = false;
                    m_LT = true;
                }
            }
            else
            {
              
                if (0.2f < m_holdLTTimer && m_holdLTTimer < 0.4f)
                {
                    Log("aaa");
                    m_justDeco = true;
                    m_LT = true;
                }

                // LTを離したらタイマーリセット
                m_holdLTTimer = 0.0f;
                m_holdRTTimer = 0.0f;
                m_targetEnemy = nullptr;
                hitEnemy->SetSlow(false);
                
            }

            // 必要時間に達したら撃破
            if (m_LT)
            {

                hitEnemy->Disable(m_justDeco);
                m_ribbon.Return();
                m_isRibbonOut = false;
                hitEnemy->SetSlow(false);
                m_totalRotation = 0.0f;
                m_isPulling = false;
                m_isRotating = false;

                // リセット
                m_holdLTTimer = 0.0f;
                m_holdRTTimer = 0.0f;
                m_targetEnemy = nullptr;
                m_LT = false;
            }
        
        }

        // Pinを掴んでいる場合の処理
//        else if (hitPin)
//        {
//            auto pinPos = hitPin->GetObject()->GetPos();
//            float dx = pinPos.x - pos.x;
//            float dy = pinPos.y - pos.y;
//            float currentDist = sqrt(dx * dx + dy * dy);
//
//            if (isRTPressed)
//            {
//                if (blockPin)
//                {
//
//                    if (m_isPulling)
//                    {
//                        //引き寄せれるPinの場合
//                        if (hitPin->GetcanRollPin())
//                        {
//                            m_isPulling = false;
//                            m_isRolling = true;
//                            float pullProgress = fabs(m_totalRotation) / (DirectX::XM_2PI * 2.0f);
//                            pullProgress = fmin(pullProgress, 1.0f);
//
//                            m_pullSpeed = 500.0f * pullProgress;
//
//                            // BlockPinのOnWindUpを呼び出す
//                            blockPin->OnWindUp({ pos.x, pos.y, pos.z }, deltaTime, m_pullSpeed);
//
//                            //BlockPinがプレイヤーに到達
//                            if (currentDist < 80.0f)
//                            {
//                                // 完了処理
//                                m_ribbon.Return();
//                                m_isRibbonOut = false;
//                                m_totalRotation = 0.0f;
//                                m_isPulling = false;
//                                m_isRolling = false;
//                                m_isRotating = false;
//                            }
//
//                        }
//                        //プレイヤーが引き寄せられる
//                        else
//                        {
//                            m_isPulling = true;
//                            m_isRolling = false;
//                            if (currentDist > 50.0f)
//                            {
//                                //m_gravity = 0.0f;
//
//                                float pullProgress = fabs(m_totalRotation) / (DirectX::XM_2PI * 2.0f);
//                                pullProgress = fmin(pullProgress, 1.0f);
//
//                                m_pullSpeed = 800.0f * pullProgress;
//
//                                // プレイヤーがPinの方向へ移動
//                                float dirX = dx / currentDist;
//                                float dirY = dy / currentDist;
//
//                                pos.x += dirX * m_pullSpeed * deltaTime;
//                                pos.y += dirY * m_pullSpeed * deltaTime;
//                            }
//                            // 2回転完了またはPinに到達
//                            if (fabs(m_totalRotation) >= DirectX::XM_2PI * 2.0f || currentDist < 80.0f)
//                            {
//                                m_ribbon.Return();
//                                m_isRibbonOut = false;
//                                m_totalRotation = 0.0f;
//                                m_isPulling = false;
//                                m_isRolling = false;
//                                m_isRotating = false;
//                            }
//                        }
//
//                    }
//
//                }
//
//                if (hitPin->GetcanDecorate())
//                {
//                    // LT長押しで敵を撃破
//                    if (isLTPressed)
//                    {
//                        // タイマー加算
//                        m_holdLTTimer += deltaTime;
//
//                        // 必要時間に達したら
//                        if (m_holdLTTimer >= m_holdLTRequired)
//                        {
//                            m_ribbon.Return();
//                            m_isRibbonOut = false;
//                            m_totalRotation = 0.0f;
//                            m_isPulling = false;
//                            m_isRotating = false;
//                            hitPin->SetState(Pin::State::Decorated);
//
//                            // リセット
//                            m_holdLTTimer = 0.0f;
//                        }
//                    }
//                    else
//                    {
//                        // LTを離したらタイマーリセット
//                        m_holdLTTimer = 0.0f;
//                    }
//                }
//            }
//            else
//            {
//                if (wasPinGrabbedLastFrame && !m_isOnGround &&!blockPin && !hitPin->GetcanRollPin() && !hitPin->GetcanDecorate())
//                {
//                    auto pinPos = hitPin->GetObject()->GetPos();
//
//                    // プレイヤーからPinへの方向ベクトル
//                    float dx = pinPos.x - pos.x;
//                    float dy = pinPos.y - pos.y;
//                    float dist = sqrt(dx * dx + dy * dy);
//
//                    if (dist > 0.0f)
//                    {
//                        // 正規化して、Pinの方向に大ジャンプ
//                       // float jumpForce = 1000.0f;  // ジャンプ力
//
//                        float minJumpForce = 400.0f;   //下限
//                        float maxJumpForce = 1200.0f;  //上限
//                        float distanceScale = 3.0f;   //距離倍率
//
//                        float jumpForce = dist * distanceScale;
//
//                        if (jumpForce < minJumpForce)
//                            jumpForce = minJumpForce;
//                        else if (jumpForce > maxJumpForce)
//                            jumpForce = maxJumpForce;
//
//                        float dirX = dx / dist;
//                        float dirY = dy / dist;
//
//                        // Pinジャンプ開始
//                        m_isPinJumping = true;
//                        m_pinJumpVelocity.x = dirX * jumpForce;
//
//                        // Y方向の速度も設定
//                        m_velY = -dirY * jumpForce * 2.5;
//                        // 重力をリセット
//                        m_gravity = 2000.0f;
//                    }
//                }
//             
//                m_ribbon.Return();
//            }
//            
//            // 最大長を超えたら自動でリボンを外す
//            if (currentDist > m_baseGuidelineLength)
//            {
//                m_ribbon.Return();
//                m_isRibbonOut = false;
//                m_totalRotation = 0.0f;
//                m_isPulling = false;
//                m_isRolling = false;
//                m_isRotating = false;
//            }
//}

// Pinを掴んでいる場合の処理
        else if (hitPin)
        {
            auto pinPos = hitPin->GetObject()->GetPos();
            float dx = pinPos.x - pos.x;
            float dy = pinPos.y - pos.y;
            float currentDist = sqrt(dx * dx + dy * dy);

            // BlockPinかどうか（距離で完了判定したい時に使う）


            if (isRTPressed)
            {
                //========================================
                // 巻き取り・引き寄せ処理（m_isPulling中）
                //========================================
                if (m_isPulling)
                {

                    // -----------------------------
                    // 1) 「巻けるピン」：ピン側が動く
                    //    BlockPin も RemoteWindPin もここに入る
                    // -----------------------------
                    if (hitPin->GetcanRollPin())
                    {
                        m_isPulling = false;
                        m_isRolling = true;

                        float pullProgress = fabs(m_totalRotation) / (DirectX::XM_2PI * 2.0f);
                        pullProgress = fmin(pullProgress, 1.0f);

                        // 速度は今の設計を踏襲
                        m_pullSpeed = 500.0f * pullProgress;

                        // ★重要：BlockPin限定をやめて Pin 基底で呼ぶ
                        hitPin->OnWindUp({ pos.x, pos.y, pos.z }, deltaTime, m_pullSpeed);

                        // 完了判定
                        // - BlockPin は「距離」で完了（既存仕様を維持）
                        // - RemoteWindPin は「完走で canRollPin=false に落ちる」想定
                        if (blockPin)
                        {
                            if (currentDist < 80.0f)
                            {
                                m_ribbon.Return();
                                m_isRibbonOut = false;
                                m_totalRotation = 0.0f;
                                m_isPulling = false;
                                m_isRolling = false;
                                m_isRotating = false;
                            }
                        }
                        else
                        {
                            // RemoteWindPin 側が完走したら SetcanRollPin(false) にする設計ならこれで確定
                            if (!hitPin->GetcanRollPin())
                            {
                                m_ribbon.Return();
                                m_isRibbonOut = false;
                                m_totalRotation = 0.0f;
                                m_isPulling = false;
                                m_isRolling = false;
                                m_isRotating = false;
                            }
                        }

                    }
                    // -----------------------------
                    // 2) 「巻けないピン」：プレイヤーが引き寄せられる（既存仕様）
                    // -----------------------------
                    else
                    {

                        if (!remotewindPin && blockPin)
                        {
                            m_isPulling = true;
                            m_isRolling = false;
                            m_isCanMove = false;
                            m_gravity = 0.0f;

                            m_circleKind == 2;

                            if (currentDist > 50.0f)
                            {
                                float pullProgress = fabs(m_totalRotation) / (DirectX::XM_2PI * 2.0f);
                                pullProgress = fmin(pullProgress, 1.0f);

                                m_pullSpeed = 800.0f * pullProgress;

                                // プレイヤーがPinの方向へ移動
                                float dirX = dx / currentDist;
                                float dirY = dy / currentDist;

                                pos.x += dirX * m_pullSpeed * deltaTime;
                                pos.y += dirY * m_pullSpeed * deltaTime;
                            }

                            //Pinに到達
                            if (currentDist < 80.0f)
                            {
                                m_ribbon.Return();
                                m_isRibbonOut = false;
                                m_totalRotation = 0.0f;
                                m_isPulling = false;
                                m_isRolling = false;
                                m_isRotating = false;
                                m_gravity = 2000.0f;
                                m_isCanMove = true;
                            }
                        }
                    }
                }


                //========================================
                // Decorate（既存仕様そのまま）
                //========================================
                if (hitPin->GetcanDecorate())
                {
                    m_circleKind = 3;

                    if (isLTPressed)
                    {
                        m_holdLTTimer += deltaTime;

                        if ((m_holdLTTimer >= m_holdLTRequired))
                        {
                            m_justDeco = false;
                            m_LT = true;
                        }     
                    }
                    else
                    {
                        if (0.2f < m_holdLTTimer && m_holdLTTimer < 0.4f)
                        {
                            Log("aaa");
                            m_justDeco = true;
                            m_LT = true;
                        }

                        m_holdLTTimer = 0.0f;
                    }

                    // 必要時間に達したら撃破
                    if (m_LT)
                    {
                        m_ribbon.Return();
                        m_isRibbonOut = false;
                        m_totalRotation = 0.0f;
                        m_isPulling = false;
                        m_isRotating = false;

                        hitPin->SetState(Pin::State::Decorated, m_justDeco);

                        // リセット
                        m_holdLTTimer = 0.0f;
                        m_holdRTTimer = 0.0f;
                        m_LT = false;
                    }
                }
            }
            else
            {
                m_circleKind = 4;

                //========================================
                // RTを離した瞬間のPinジャンプ（既存仕様そのまま）
                //========================================
                if ((wasPinGrabbedLastFrame && (!m_isOnGround || leftStickUpTrigger) && !blockPin && !remotewindPin) &&
                    !hitPin->GetcanRollPin() && !hitPin->GetcanDecorate())
                {
                    m_isCanMove = true;
                    m_isPulling = false;
                    m_isRolling = false;
                    auto ppos = hitPin->GetObject()->GetPos();

                    float jdx = ppos.x - pos.x;
                    float jdy = ppos.y - pos.y;
                    float dist = sqrt(jdx * jdx + jdy * jdy);

                    if (dist > 0.0f)
                    {
                        float minJumpForce = 800.0f;
                        float maxJumpForce = 1200.0f;
                        float distanceScale = 3.0f;

                        float jumpForce = dist * distanceScale;
                        if (jumpForce < minJumpForce)
                        {
                            jumpForce = minJumpForce;
                        }
                        else if (jumpForce > maxJumpForce)
                        {
                            jumpForce = maxJumpForce;
                        }


                        //float dirX = jdx / dist;
                        float dirY = jdy / dist;

                        // X方向は一定の強さ・方向
                        float XForce = 1400.0f;
                        float dirX = (jdx > 0) ? 1.0f : -1.0f;

                        // Pinジャンプ開始
                        m_isPinJumping = true;
                        m_pinJumpVelocity.x = dirX * XForce;

                        // Y方向の速度も設定
                        m_velY = -dirY * (jumpForce / 2) * 2.5f;

                        // 重力をリセット
                        m_gravity = 2000.0f;
                    }
                }

                m_ribbon.Return();
                m_isCanMove = true;
            }

            //========================================
            // 最大長を超えたら自動でリボンを外す（既存）
            //========================================
            if (currentDist > m_baseGuidelineLength)
            {
                m_ribbon.Return();
                m_isRibbonOut = false;
                m_totalRotation = 0.0f;
                m_isPulling = false;
                m_isRolling = false;
                m_isRotating = false;
            }
        }

        else if (!hitEnemy && !hitPin)
        {
            // 何も掴んでいない場合はリセット
            m_gravity = 2000.0f;
            m_totalRotation = 0.0f;
            m_isPulling = false;
            m_isRolling = false;
            m_isRotating = false;
            // 敵を掴んでいない時は凍結解除
            for (auto enemy : enemies)
            {
                if (enemy)
                    enemy->SetSlow(false);
            }

        }
    }
    else
    {
        pos.x += m_knockbackVelocity.x * deltaTime;
        pos.y -= m_knockbackVelocity.y * deltaTime;
        m_knockbackVelocity.x *= 0.95f;
        m_knockbackVelocity.y *= 0.95f;
    }

    // ジャンプ
    if (m_isOnGround &&
        (input.GetKeyTrigger(VK_SPACE)
            || input.GetButtonTrigger(XINPUT_A)
            || input.GetButtonTrigger(XINPUT_UP)
            || leftStickUpTrigger)
        )
    {
        m_velY = -m_jumpPower;
        m_isOnGround = false;
    }

    //========================================
    // 右スティックを倒すと指示線表示 + オートエイム
    //========================================
    Enemy* bestEnemy = nullptr;

    if (aiming)
    {
        //====================================
        // プレイヤーの位置を取得
        //====================================
        auto p = m_player.GetPos();

        //====================================
        // 右スティック方向を角度に変換
        //====================================
        float angleRad = atan2(m_aimDirection.y, m_aimDirection.x);
        float guideAngleRad = atan2(m_aimDirection.y, m_aimDirection.x);

        // 指示線の向きベクトル
        float dirX = cos(angleRad);
        float dirY = sin(angleRad);

        // 指示線の初期長さ
        float guidelineLength = m_baseGuidelineLength;

        // 指示線からの垂直距離（最小）
        float minPerpendicularDist = FLT_MAX;



        // ±20度の範囲判定用
        float angleThreshold = cos(20.0f * DirectX::XM_PI / 180.0f);

        //------------------------------------
        // 地面とのレイキャストチェック
        //------------------------------------
        float groundHitDistance = m_baseGuidelineLength;
        bool hitGround = false;

        // レイ分割数（精度）
        const int raySteps = 20;
        const float stepSize = m_baseGuidelineLength / raySteps;

        for (int i = 1; i <= raySteps; ++i)
        {
            float checkDist = stepSize * i;
            float checkX = p.x + dirX * checkDist;
            float checkY = p.y + dirY * checkDist;

            // レイの一点を小さなAABBとして扱う
            CollisionManager::AABB rayPoint;
            const float pointSize = 5.0f;
            rayPoint.min = { checkX - pointSize, checkY - pointSize };
            rayPoint.max = { checkX + pointSize, checkY + pointSize };

            // 地面に当たったらそこまで
            if (m_collisionMgr && m_collisionMgr->CheckHitStatic(rayPoint))
            {
                groundHitDistance = checkDist;
                hitGround = true;
                break;
            }
        }

        //------------------------------------
        // 敵探索（オートエイム）
        //------------------------------------
        for (auto enemy : enemies)
        {
            if (!enemy) continue;

            auto enemyPos = enemy->GetObject()->GetPos();

            // プレイヤー→敵ベクトル
            float toX = enemyPos.x - p.x;
            float toY = enemyPos.y - p.y;
            float dist = sqrt(toX * toX + toY * toY);

            // 指示線の最大長を超えていたら無視
            if (dist > guidelineLength)
                continue;

            // 方向一致度（内積）
            float dot = (dirX * toX + dirY * toY) / dist;

            m_circleKind = 0;

            // ±20度以内か
            if (dot > angleThreshold)
            {
                // 指示線方向への投影距離
                float projectionLength = dot * dist;

                // 指示線の前方向 & 範囲内か
                if (projectionLength < 0.0f || projectionLength > guidelineLength)
                    continue;

                // 指示線からの垂直距離
                float perpendicularDist =
                    sqrt(dist * dist - projectionLength * projectionLength);

                //--------------------------------
                // 壁で遮られていないかチェック
                //--------------------------------
                bool blocked = false;
                if (m_collisionMgr)
                {
                    float ex = toX / dist;
                    float ey = toY / dist;

                    const int steps = 10;
                    const float step = dist / steps;

                    for (int j = 1; j < steps - 1; ++j)
                    {
                        float cx = p.x + ex * step * j;
                        float cy = p.y + ey * step * j;

                        CollisionManager::AABB ray;
                        ray.min = { cx - 2, cy - 2 };
                        ray.max = { cx + 2, cy + 2 };

                        if (m_collisionMgr->CheckHitStatic(ray))
                        {
                            blocked = true;
                            break;
                        }
                    }
                }

                //--------------------------------
                // 最も指示線に近い敵を採用
                //--------------------------------
                if (!blocked && perpendicularDist < minPerpendicularDist)
                {
                    minPerpendicularDist = perpendicularDist;
                    guidelineLength = projectionLength;
                    foundEnemy = true;
                    m_targetPosition = enemyPos;
                    bestEnemy = enemy;
                }
            }
        }

        //------------------------------------
        // Pin探索（オートエイム）
        //------------------------------------
        for (auto pin : pins)
        {
            if (!pin) continue;
            if ((pin->GetState() == Pin::State::Decorated)) continue;

            auto pinPos = pin->GetObject()->GetPos();

            // プレイヤー→Pinベクトル
            float toX = pinPos.x - p.x;
            float toY = pinPos.y - p.y;
            float dist = sqrt(toX * toX + toY * toY);

            // 指示線の最大長を超えていたら無視
            if (dist > guidelineLength)
                continue;

            // 方向一致度（内積）
            float dot = (dirX * toX + dirY * toY) / dist;


            switch(pin->GetPinKind())
            {
            case PinKind::Roll:
                m_circleKind = 1;
                break;

            case PinKind::Pulled:
                m_circleKind = 2;
                break;

            case  PinKind::Deco:
                m_circleKind = 3;
                break;

            case  PinKind::Jump:
                m_circleKind = 4;
                break;

            case  PinKind::RemoteDeco:
                m_circleKind = 5;
                break;

            default:
                m_circleKind = 0;
                break;
            }
         
         

            // ±20度以内か
            if (dot > angleThreshold)
            {
                // 指示線方向への投影距離
                float projectionLength = dot * dist;

                // 指示線の前方向 & 範囲内か
                if (projectionLength < 0.0f || projectionLength > guidelineLength)
                    continue;

                const float pinAimRadius = 50.0f;

                // 指示線からの垂直距離
                float perpendicularDist =
                    sqrt(dist * dist - projectionLength * projectionLength);

                //--------------------------------
                // 壁で遮られていないかチェック
                //--------------------------------
                bool blocked = false;
                if (m_collisionMgr)
                {
                    float ex = toX / dist;
                    float ey = toY / dist;

                    const int steps = 10;
                    const float step = dist / steps;

                    for (int j = 1; j < steps - 1; ++j)
                    {
                        float cx = p.x + ex * step * j;
                        float cy = p.y + ey * step * j;

                        CollisionManager::AABB ray;
                        ray.min = { cx - 2, cy - 2 };
                        ray.max = { cx + 2, cy + 2 };

                        if (m_collisionMgr->CheckHitStatic(ray))
                        {
                            blocked = true;
                            break;
                        }
                    }
                }

                //--------------------------------
                // 最も指示線に近いPinを採用
                //--------------------------------
                if (!blocked &&
                    perpendicularDist < pinAimRadius &&
                    perpendicularDist < minPerpendicularDist)
                {
                    minPerpendicularDist = perpendicularDist;
                    guidelineLength = projectionLength;
                    foundPin = true;  // Pinも発見扱い
                    m_targetPosition = pinPos;
                }
            }
        }

        //------------------------------------
        // 敵がいなければ地面まで伸ばす
        //------------------------------------
        if (!foundEnemy && !foundPin && hitGround)
            guidelineLength = groundHitDistance;

        //------------------------------------
        // 見た目・ターゲット処理
        //------------------------------------
        m_hasTarget = foundEnemy;

        if (foundEnemy || foundPin)
        {
            float toTargetX = m_targetPosition.x - p.x;
            float toTargetY = m_targetPosition.y - p.y;
            float distToTarget = sqrt(toTargetX * toTargetX + toTargetY * toTargetY);

            if (distToTarget > 0.0f)
            {
                dirX = toTargetX / distToTarget;
                dirY = toTargetY / distToTarget;
                angleRad = atan2(dirY, dirX);
            }

            // ターゲットあり色
            m_guideline.SetColor(1, 0.5f, 0.5f, 0.8f);

            // ターゲット円表示
            m_Circle.SetPos(m_targetPosition.x, m_targetPosition.y, 0);

            //円の色の種類分け
            switch (m_circleKind)
            {
                //巻取り用
            case 1:
                m_Circle.SetColor(1.0f, 0.75f, 0.8f, 1.0f);
                break;

                //巻き取られ用
            case 2:
                m_Circle.SetColor(0.0f, 1.0f, 0.0f, 1.0f);
                break;
            
            //飾れるピン用
            case 3:
                m_Circle.SetColor(1.0f, 1.0f, 0.0f, 1.0f);
                break;
           
            case 4:
                m_Circle.SetColor(1.0f, 0.65f, 0.0f, 1.0f);
                break;

            case 5:
                m_Circle.SetColor(0.0f, 1.0f, 1.0f, 1.0f);
                break;

            default:
                m_Circle.SetColor(1.0f, 1.0f, 1.0f, 1.0f);
                break;
            }
        }
        else
        {
            // 通常色
            m_guideline.SetColor(1, 1, 1, 0.5f);
            m_Circle.SetColor(1, 1, 1, 0);
        }

        //------------------------------------
        // ガイドライン描画
        //------------------------------------
        if (m_ribbon.GetState() != Ribbon::State::Throwing && m_ribbon.GetState() != Ribbon::State::Holding)
        {
            // 発射基準点（胸あたり）
            float originX = p.x;
            float originY = p.y;

            // ガイドライン中心位置
            m_guideline.SetPos(
                originX + dirX * (guidelineLength * 0.5f),
                originY + dirY * (guidelineLength * 0.5f),
                p.z
            );

            // サイズ
            m_guideline.SetSize(guidelineLength, 20, 0);

            // 回転
            m_guideline.SetAngle(angleRad * 180.0f / DirectX::XM_PI);
            m_guideline.Update(deltaTime);
        }
        else
        {
            // リボン中は非表示
            m_guideline.SetColor(1, 1, 1, 0);
            m_Circle.SetColor(1, 1, 1, 0);
        }

        //------------------------------------
        // リボン用に情報保存
        //------------------------------------
        m_ribbonAimDir = { dirX, dirY };
        m_ribbonAimLength = guidelineLength;
        m_ribbonTargetEnemy = foundEnemy ? bestEnemy : nullptr;

        // 向き更新
        if (!isRTPressed)
        {
            m_isLastRightDirection = (dirX > 0);
        }

    }
    else
    {
        // エイムしていないときは非表示
        m_guideline.SetColor(1, 1, 1, 0);
        m_Circle.SetColor(1, 1, 1, 0);
        m_ribbonTargetEnemy = nullptr;
    }

    //========================================
    // リボン更新
    //========================================
    m_ribbon.SetPlayerPos({ pos.x, pos.y });
    m_ribbon.Update(deltaTime, enemies, pins);


    //========================================
    // リボン発射処理
    //========================================


    if (!m_isKnockback)
    {
        if (isRTPressed)
        {
            if (!hitPin)
            {
                if (hitEnemy)
                {
                    //LTを押していない,リボンが何も当たっていない場合
                    if (isLTPressed)
                    {
                        // タイマー加算
                        m_holdRTTimer -= deltaTime;
                    }
                }
                m_holdRTTimer += deltaTime;
            }
        }



        if (m_holdRTTimer >= m_holdRTRequired)
        {
            m_ribbon.Return();
            m_isRibbonOut = false;
            m_totalRotation = 0.0f;
            m_isPulling = false;
            m_isRotating = false;

            // リセット
            m_holdRTTimer = 0.0f;
        }

        // 発射入力
        if ((input.GetKeyTrigger(VK_X) || (isRTPressed && !m_wasRTPressed)) && !m_isRibbonOut)
        {
            if (aiming)
            {
                // ターゲットがある場合は、ターゲット位置への方向ベクトルを計算
                if (foundEnemy || foundPin)
                {
                    DirectX::XMFLOAT2 toTarget;
                    toTarget.x = m_targetPosition.x - pos.x;
                    toTarget.y = m_targetPosition.y - pos.y;
                    float dist = sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);


                    if (dist > 0.0f)
                    {
                        toTarget.x /= dist;
                        toTarget.y /= dist;
                    }

                    m_ribbon.Throw(toTarget, dist);
                }
                else
                {
                    m_ribbon.Throw(m_ribbonAimDir, m_ribbonAimLength);
                }
            }
            else
            {
                // 最後に向いていた方向へ発射
                m_ribbon.Throw(
                    m_isLastRightDirection
                    ? DirectX::XMFLOAT2{ 1,0 }
                    : DirectX::XMFLOAT2{ -1,0 },
                    m_baseGuidelineLength
                );
            }

            m_isRibbonOut = true;
        }

        // 戻し処理
        if (m_ribbon.GetState() == Ribbon::State::Returning || (!isRTPressed && m_wasRTPressed))
        {
            if (m_isRibbonOut)
            {
                m_ribbon.Return();
                m_isRibbonOut = false;
            }
        }
    }

    // 次フレーム用
    m_wasRTPressed = isRTPressed;
    m_prevLeftStickUp = stickUpNow;

    if (!isRTPressed)
    {

        if (m_inputDir != 0)
        {
            m_isLastRightDirection = (m_inputDir == -1);
        }
        else if (aiming)
        {
            m_isLastRightDirection = (m_aimDirection.x > 0);
        }
        else if (m_isThrowAnimLock)
        {
            m_isLastRightDirection = m_throwDirectionRight;
        }
    }
    else if (m_inputDir != 0)
    {
        m_isLastRightDirection = (m_inputDir == -1);
    }

    if (m_isPulling || m_isRolling)
    {
        auto ppos = hitPin->GetObject()->GetPos();

        if (ppos.x < pos.x)
        {
            m_isLastRightDirection = false;
        }
        else if (ppos.x > pos.x)
        {
            m_isLastRightDirection = true;
        }
    }


    if (m_isKnockback)            m_animState = Damage;
    else if (m_isPinJumping)      m_animState = RibbonJump;
    else if ((m_isPulling || !m_isOnGround)&&  !m_isCanMove)         m_animState = Pulled;
    else if (m_isRolling && (blockPin || remotewindPin))         m_animState = Roll;
    else if (!m_isOnGround && (!m_isRolling || !m_isPulling) && m_isCanMove)       m_animState = Jump;
    else if (m_inputDir != 0)     m_animState = Run;
    else if (m_ribbon.GetState() == Ribbon::State::Throwing)   m_animState = Throw;

    else                          m_animState = Idle;

    //実行
    ApplyAnimation();
    m_player.SetPos(pos.x, pos.y, pos.z);
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

void Player::ApplyAnimation()
{
    // アニメーション状態が変わった場合のみ切り替え
    if (m_animState != m_prevAnimState)
    {
        switch (m_animState)
        {
        case Idle:
            m_player.PlayAnimation(
                m_isLastRightDirection ? "RightIdle" : "LeftIdle"
            );
            break;

        case Run:
            m_player.PlayAnimation(
                m_isLastRightDirection ? "Right" : "Left"
            );
            break;

        case Jump:
            m_player.PlayAnimation(
                m_isLastRightDirection ? "RJump" : "LJump"
            );
            break;

        case Damage:
            m_player.PlayAnimation(
                m_isLastRightDirection ? "RDamage" : "LDamage"
            );
            break;

        case Throw:
            m_player.PlayAnimation(
                m_isLastRightDirection ? "ROutLibbon" : "LOutLibbon"
            );
            break;
        case RibbonJump:
            m_player.PlayAnimation(
                m_isLastRightDirection ? "RRibbonJump" : "LRibbonJump"
            );
            break;
        case Roll:
            m_player.PlayAnimation(
                m_isLastRightDirection ? "RRoll" : "LRoll"
            );
            break;
        case Pulled:
            m_player.PlayAnimation(
                m_isLastRightDirection ? "RPulled" : "LPulled"
            );
            break;
        }

        m_prevAnimState = m_animState;
    }

    // 向きだけ変わった場合は、アニメーション名だけ切り替え（フレームは維持）
    if (m_isLastRightDirection != m_prevIsRightDirection)
    {
        switch (m_animState)
        {
        case Idle:
            m_player.SetCurrentAnimationName(
                m_isLastRightDirection ? "RightIdle" : "LeftIdle"
            );
            break;

        case Run:
            m_player.SetCurrentAnimationName(
                m_isLastRightDirection ? "Right" : "Left"
            );
            break;

        case Jump:
            m_player.SetCurrentAnimationName(
                m_isLastRightDirection ? "RJump" : "LJump"
            );
            break;

        case Damage:
            m_player.SetCurrentAnimationName(
                m_isLastRightDirection ? "RDamage" : "LDamage"
            );
            break;

        case Throw:
            m_player.SetCurrentAnimationName(
                m_isLastRightDirection ? "ROutLibbon" : "LOutLibbon"
            );
            break;
        case RibbonJump:
            m_player.SetCurrentAnimationName(
                m_isLastRightDirection ? "RRibbonJump" : "LRibbonJump"
            );
            break;
        case Roll:
            m_player.SetCurrentAnimationName(
                m_isLastRightDirection ? "RRoll" : "LRoll"
            );
            break;
        case Pulled:
            m_player.SetCurrentAnimationName(
                m_isLastRightDirection ? "RPulled" : "LPulled"
            );
            break;
        }

        m_prevIsRightDirection = m_isLastRightDirection;
    }
}

//RibbonにCollisionManagerを渡すために.hから.cppへ移動
void Player::SetCollisionManager(CollisionManager* mgr)
{
    m_collisionMgr = mgr;

    m_ribbon.SetCollisionManager(mgr);
}
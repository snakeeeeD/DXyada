#include "Player.h"
#include "Input.h"
#include "Renderer.h"
#include "Texture.h"
#include "Object.h"
#include "framework.h"
#include "CollisionManager.h"


extern Input input;
DirectX::XMFLOAT3 g_StartPlayer = { 0,0,0 };

void Player::Init() {

    m_player.Init();
    m_player.SetPos(g_StartPlayer.x, g_StartPlayer.y, 0);
    m_player.SetSize(100, m_height, 0);

    // アニメーション設定
    m_player.AddAnimation("Left", "asset/Player_Work.png", 4, 1, 0, 0, 1, 5, true, 1);
    m_player.AddAnimation("Right", "asset/Player_Work.png", 4, 1, 0, 2, 3, 5, true, 1);
    m_player.AddAnimation("LeftIdle", "asset/Player_Idle.png",  4, 2, 0, 0, 3, 5, true, 1);
    m_player.AddAnimation("RightIdle", "asset/Player_Idle.png", 4, 2, 1, 0, 3, 5, true, 1);
    m_player.AddAnimation("LJump", "asset/Player_SmallJump.png", 5, 2, 0, 0, 4, 9, false, 2);
    m_player.AddAnimation("RJump", "asset/Player_SmallJump.png", 5, 2, 1, 0, 4, 9, false, 2);
    m_player.AddAnimation("LDamage", "asset/Player_Damage.png", 5, 2, 0, 0, 4, 9, false, 3);
    m_player.AddAnimation("RDamage", "asset/Player_Damage.png", 5, 2, 1, 0, 4, 9, false, 3);
    m_player.AddAnimation("LOutLibbon", "asset/Player_Ribbon.png", 5, 2, 0, 0, 4, 15, false, 2);
    m_player.AddAnimation("ROutLibbon", "asset/Player_Ribbon.png", 5, 2, 1, 0, 4, 15, false, 2);

    m_animState = Idle;
    m_prevAnimState = static_cast<Player_AnimState>(-1);

    m_prevIsRightDirection = m_isLastRightDirection;
    m_isThrowingThisFrame = false;
    m_waitReleaseAfterThrow = false;

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

void Player::Update(
    float deltaTime,
    const std::vector<Platform>& platforms,
    std::vector<Enemy*>& enemies
)
{
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
    auto pos = m_player.GetPos();   //プレイヤーの位置を取得
 //--------------------------------------
 // エイム入力（右スティック or マウス）
 //--------------------------------------
    DirectX::XMFLOAT2 leftStick = input.GetLeftAnalogStick();// 移動専用
    DirectX::XMFLOAT2 rightStick = input.GetRightAnalogStick();// エイム専用
    const float moveThreshold = 0.5f;

    // 右スティック判定
    bool stickAiming =
        fabs(rightStick.x) > moveThreshold ||
        fabs(rightStick.y) > moveThreshold;

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


    // 重力を加算
    m_velY += m_gravity * deltaTime;
    pos.y -= m_velY * deltaTime;

    m_isOnGround = false;

    // Platform上の着地判定
    for (auto& plat : platforms)
    {
        auto pPos = plat.GetObject()->GetPos();
        auto pSize = plat.GetObject()->GetSize();

        float platTop = pPos.y + pSize.y * 0.5f;
        float platLeft = pPos.x - pSize.x * 0.5f;
        float platRight = pPos.x + pSize.x * 0.5f;

        float playerBottom = pos.y - m_height * 0.5f;
        float playerLeft = pos.x - m_width * 0.5f;
        float playerRight = pos.x + m_width * 0.5f;

        // 横方向が重なるか
        if (playerRight > platLeft && playerLeft < platRight)
        {
            // 下端が上面に接触しているか
            if (playerBottom <= platTop && playerBottom >= platTop - 10.0f)
            {
                pos.y = platTop + m_height * 0.5f;
                m_velY = 0.0f;
                m_isOnGround = true;
            }
        }
    }

    //操作関係
    bool moveLeft =
        input.GetKeyPress(VK_A) ||
        input.GetButtonPress(XINPUT_LEFT) ||
        (leftStick.x < -moveThreshold);

    bool moveRight =
        input.GetKeyPress(VK_D) ||
        input.GetButtonPress(XINPUT_RIGHT) ||
        (leftStick.x > moveThreshold);

    //ノックバック中出ない場合のみ移動
    if (!m_isKnockback)
    {
        if (moveLeft && !moveRight)       m_inputDir = 1;
        else if (moveRight && !moveLeft)  m_inputDir = -1;
        else                              m_inputDir = 0;

        if (m_inputDir == 1)  pos.x -= 200.0f * deltaTime;
        if (m_inputDir == -1) pos.x += 200.0f * deltaTime;
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
        (input.GetKeyTrigger(VK_SPACE) || input.GetButtonTrigger(XINPUT_A)))
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
        float guideAngleRad = angleRad;

        // 指示線の向きベクトル
        float dirX = cos(angleRad);
        float dirY = sin(angleRad);

        // 指示線の初期長さ
        float guidelineLength = m_baseGuidelineLength;

        // 指示線からの垂直距離（最小）
        float minPerpendicularDist = FLT_MAX;

        bool foundEnemy = false;

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

            // ±20度以内か
            if (dot > angleThreshold)
            {
                // 指示線方向への投影距離
                float projectionLength = dot * dist;

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
                        ray.min = { cx - 5, cy - 5 };
                        ray.max = { cx + 5, cy + 5 };

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
        // 敵がいなければ地面まで伸ばす
        //------------------------------------
        if (!foundEnemy && hitGround)
            guidelineLength = groundHitDistance;

        //------------------------------------
        // 見た目・ターゲット処理
        //------------------------------------
        m_hasTarget = foundEnemy;

        if (foundEnemy)
        {
            // プレイヤー→敵方向に補正
            float tx = m_targetPosition.x - p.x;
            float ty = m_targetPosition.y - p.y;

            angleRad = atan2(ty, tx);
            dirX = cos(angleRad);
            dirY = sin(angleRad);

            // ターゲットあり色
            m_guideline.SetColor(1, 0.5f, 0.5f, 0.8f);

            // ターゲット円表示
            m_Circle.SetPos(m_targetPosition.x, m_targetPosition.y, 0);
            m_Circle.SetColor(1, 1, 1, 1);
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
        if (m_ribbon.GetState() != Ribbon::State::Throwing)
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
        }

        //------------------------------------
        // リボン用に情報保存
        //------------------------------------
        m_ribbonAimDir = { dirX, dirY };
        m_ribbonAimLength = guidelineLength;
        m_ribbonTargetEnemy = foundEnemy ? bestEnemy : nullptr;

        // 向き更新
        m_isLastRightDirection = (dirX > 0);
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
    m_ribbon.Update(deltaTime, enemies);
    //========================================
    // リボン先端ヒット判定
    //========================================
    Enemy* hitEnemy = m_ribbon.GetHitEnemy();
    if (hitEnemy)
    {
        hitEnemy->Disable();
        m_ribbon.Return();
    }

    //========================================
    // リボン発射処理
    //========================================
    float rightTrigger = input.GetRightTrigger();
    bool isRTPressed = rightTrigger > 0.5f;

    if (!m_isKnockback)
    {
        // 発射入力
        if ((input.GetKeyTrigger(VK_X) || (isRTPressed && !m_wasRTPressed)) && !m_isRibbonOut)
        {
            if (aiming)
            {
                // エイム方向へ発射
                m_ribbon.Throw(m_ribbonAimDir, m_ribbonAimLength);
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

    /*
   // 右スティックを倒すと指示線表示
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

        //地面との衝突チェック
        float groundHitDistance = m_baseGuidelineLength;
        bool hitGround = false;

        //レイキャストのステップ数(精度)
        const int raySteps = 20;
        const float stepSize = m_baseGuidelineLength / raySteps;

        for (int i = 1; i <= raySteps; ++i)
        {
            float checkDist = stepSize * i;
            float checkX = p.x + dirX * checkDist;
            float checkY = p.y + dirY * checkDist;

            //チェック地点のAABBを作成(小さな点として)
            CollisionManager::AABB rayPoint;
            const float pointSize = 5.0f; // 判定用の小さなサイズ
            rayPoint.min = { checkX - pointSize, checkY - pointSize };
            rayPoint.max = { checkX + pointSize, checkY + pointSize };

            // CollisionManagerを使って地面との衝突をチェック
            if (m_collisionMgr && m_collisionMgr->CheckHitStatic(rayPoint))
            {
                groundHitDistance = checkDist;
                hitGround = true;
                break;
            }
        }

        for (const auto& enemy : Enemy)
        {
            auto enemyPos = enemy->GetObject()->GetPos();

            float toenemyX = enemyPos.x - p.x;
            float toenemyY = enemyPos.y - p.y;
            float distToEnemy = sqrt(toenemyX * toenemyX + toenemyY * toenemyY);

            if (distToEnemy > guidelineLength) continue; // 地面までの距離を考慮

            float dotProduct = (dirX * toenemyX + dirY * toenemyY) / distToEnemy;

            if (dotProduct > angleThreshold)
            {
                //指示線からの垂直距離を計算
                float projectionLength = dotProduct * distToEnemy;  //指示線の投影距離
                float perpendicularDist = sqrt(distToEnemy * distToEnemy - projectionLength * projectionLength);

                // プレイヤーから敵までの経路上に壁がないかチェック
                bool pathBlocked = false;
                if (m_collisionMgr)
                {
                    float enemyDirX = toenemyX / distToEnemy;
                    float enemyDirY = toenemyY / distToEnemy;

                    const int pathCheckSteps = 10;
                    const float pathStepSize = distToEnemy / pathCheckSteps;

                    for (int j = 1; j < pathCheckSteps - 1; ++j)
                    {
                        float checkDist = pathStepSize * j;
                        float checkX = p.x + enemyDirX * checkDist;
                        float checkY = p.y + enemyDirY * checkDist;

                        CollisionManager::AABB rayPoint;
                        const float pointSize = 5.0f;
                        rayPoint.min = { checkX - pointSize, checkY - pointSize };
                        rayPoint.max = { checkX + pointSize, checkY + pointSize };

                        if (m_collisionMgr->CheckHitStatic(rayPoint))
                        {
                            pathBlocked = true;
                            break;
                        }
                    }
                }

                //指示線から一番近い敵を選択（壁に遮られていない場合のみ）
                if (!pathBlocked && perpendicularDist < minPerpendicularDist && projectionLength <= guidelineLength)
                {
                    minPerpendicularDist = perpendicularDist;
                    guidelineLength = projectionLength;
                    foundEnemy = true;
                    m_targetPosition = enemyPos;
                }
            }
        }

        // 敵が見つからなかった場合は地面までの距離を使用
        if (!foundEnemy && hitGround)
        {
            guidelineLength = groundHitDistance;
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
            m_guideline.SetPos(p.x + (guidelineLength / 2), p.y, p.z);

            // 指示線の長さを調整
            m_guideline.SetSize(guidelineLength, 20, 0);

            //ラジアンを度へ変換
            float angleDeg = angleRad * (180.0f / DirectX::XM_PI);
            float GuideangleDeg = GuideangleRad * (180.0f / DirectX::XM_PI);

            //回転の中心を左端に移動
            m_guideline.SetPivot(((-guidelineLength) / 2), 0, 0);

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

            if (fabs(rightStick.x) > 0.3f)
            {
                m_isLastRightDirection = (rightStick.x > 0);
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
                        //m_player.PlayAnimation("ROutLibbon");
                    }
                    else if (rightStick.x < 0)
                    {
                        //m_player.PlayAnimation("LOutLibbon");
                    }
                  
                }
                else
                {
                    // 右スティックが倒されていない場合は最後に向いた方向
                    if (m_isLastRightDirection)
                    {
                        m_ribbon.Throw({ 1.0f, 0.0f }); // 右方向
                        //m_player.PlayAnimation("ROutLibbon");
                    }
                    else
                    {
                        m_ribbon.Throw({ -1.0f, 0.0f }); // 左方向
                        //m_player.PlayAnimation("LOutLibbon");
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

    m_wasRTPressed = isRTPressed;
    */
    // 向き決定
    if (m_isThrowAnimLock)
    {
        m_isLastRightDirection = m_throwDirectionRight;
    }
    else if (m_inputDir != 0)
    {
        m_isLastRightDirection = (m_inputDir == -1);
    }
    else if (aiming)
    {
        m_isLastRightDirection = (m_aimDirection.x > 0);
    }

    if (m_isKnockback)            m_animState = Damage;
    else if (m_isThrowAnimLock)   m_animState = Throw;
    else if (!m_isOnGround)       m_animState = Jump;
    else if (m_inputDir != 0)     m_animState = Run;
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
    if (m_animState == m_prevAnimState &&
        m_isLastRightDirection == m_prevIsRightDirection)
        return;

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
    }

    m_prevAnimState = m_animState;
    m_prevIsRightDirection = m_isLastRightDirection;
}

#include "DecoMarker.h"

void DecoMarker::Init(const char* tex)
{
    m_obj.Init();
    m_obj.AddTexture(tex);

    m_obj2.Init();
    m_obj2.AddTexture(tex);

    m_obj3.Init();
    m_obj3.AddTexture(tex);
    Hide();
}

void DecoMarker::Hide()
{
    m_obj.SetSize(0, 0, 0);
    m_obj2.SetSize(0, 0, 0);
    m_obj3.SetSize(0, 0, 0);
    m_visible = false;
}

void DecoMarker::ShowAtHead(Object* target, float offsetY, float w, float h)
{
    if (!target) return;

    auto pos = target->GetPos();
    auto size = target->GetSize();

    //// 頭上に配置
    //m_obj.SetPos(
    //    pos.x,
    //    pos.y + size.y * 0.5f + offsetY,
    //    pos.z
    //);

    //m_obj.SetSize(w, h, 0);

    //// ★ 角度をランダムに1回だけ付ける
    //int angle = (rand() % 41) - 20;   // -30 ～ +30
    //m_obj.SetAngle(static_cast<float>(angle));

    if (!m_isMove)
    {
        // ★Pinのように動く場合: 6つの配置候補からランダム選択
        struct MarkPosition {
            float offsetX;
            float offsetY;
            float angle;
        };

        MarkPosition positions[6] = {
            // グループ1 (m_obj用の2つ)
            { -30.0f, offsetY, -15.0f },
            { -20.0f, offsetY + 10.0f, 10.0f },

            // グループ2 (m_obj2用の2つ)
            { 0.0f, offsetY + 5.0f, -5.0f },
            { 5.0f, offsetY + 15.0f, 20.0f },

            // グループ3 (m_obj3用の2つ)
            { 25.0f, offsetY, -20.0f },
            { 30.0f, offsetY + 10.0f, 15.0f }
        };

        //m_obj: グループ1からランダム選択 (0 or 1)
        int idx1 = rand() % 2;
        m_offsetX1 = positions[idx1].offsetX;
        m_offsetY1 = positions[idx1].offsetY;
        m_obj.SetAngle(positions[idx1].angle);
        m_obj.SetPos(pos.x + m_offsetX1, pos.y + size.y * 0.5f + m_offsetY1, pos.z);
        m_obj.SetSize(w, h, 0);

        //m_obj2: グループ2からランダム選択 (2 or 3)
        int idx2 = 2 + (rand() % 2);
        m_offsetX2 = positions[idx2].offsetX;
        m_offsetY2 = positions[idx2].offsetY;
        m_obj2.SetAngle(positions[idx2].angle);
        m_obj2.SetPos(pos.x + m_offsetX2, pos.y + size.y * 0.5f + m_offsetY2, pos.z);
        m_obj2.SetSize(w, h, 0);

        //m_obj3: グループ3からランダム選択 (4 or 5)
        int idx3 = 4 + (rand() % 2);
        m_offsetX3 = positions[idx3].offsetX;
        m_offsetY3 = positions[idx3].offsetY;
        m_obj3.SetAngle(positions[idx3].angle);
        m_obj3.SetPos(pos.x + m_offsetX3, pos.y + size.y * 0.5f + m_offsetY3, pos.z);
        m_obj3.SetSize(w, h, 0);
    }
    else
    {
        //元の仕様: 1つだけランダム角度で頭上に配置
        m_obj.SetPos(
            pos.x,
            pos.y + size.y * 0.5f + offsetY,
            pos.z
        );
        m_obj.SetSize(w, h, 0);

        int angle = (rand() % 41) - 20;   // -20 ～ +20
        m_obj.SetAngle(static_cast<float>(angle));

        // 他の2つは非表示
        m_obj2.SetSize(0, 0, 0);
        m_obj3.SetSize(0, 0, 0);
    }

    m_visible = true;
}

void DecoMarker::Update(float dt, Object* target)
{
    if (m_isMove && target)
    {
        auto pos = target->GetPos();
        auto size = target->GetSize();

        m_obj.SetPos(pos.x + m_offsetX1, pos.y + size.y * 0.5f + m_offsetY1, pos.z);
        m_obj2.SetPos(pos.x + m_offsetX2, pos.y + size.y * 0.5f + m_offsetY2, pos.z);
        m_obj3.SetPos(pos.x + m_offsetX3, pos.y + size.y * 0.5f + m_offsetY3, pos.z);
    }

    m_obj.Update(dt);
    m_obj2.Update(dt);
    m_obj3.Update(dt);
}


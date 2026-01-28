#include "DecoMarker.h"

void DecoMarker::Init(const char* tex)
{
    m_obj.Init();
    m_obj.AddTexture(tex);
    Hide();
}

void DecoMarker::Hide()
{
    m_obj.SetSize(0, 0, 0);
    m_visible = false;
}

void DecoMarker::ShowAtHead(Object* target, float offsetY, float w, float h)
{
    if (!target) return;

    auto pos = target->GetPos();
    auto size = target->GetSize();

    // 頭上に配置
    m_obj.SetPos(
        pos.x,
        pos.y + size.y * 0.5f + offsetY,
        pos.z
    );

    m_obj.SetSize(w, h, 0);

    // ★ 角度をランダムに1回だけ付ける
    int angle = (rand() % 41) - 20;   // -30 ～ +30
    m_obj.SetAngle(static_cast<float>(angle));

    m_visible = true;
}

void DecoMarker::Update(float dt)
{
    m_obj.Update(dt);
}

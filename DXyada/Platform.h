#pragma once
#include "Object.h"

class Platform {
private:
    Object m_object;

public:
    Platform() = default;
    ~Platform() = default;

    void Init();
    void Init(const char* texture, float x, float y, float width, float height);

    Object* GetObject() { return &m_object; }
    const Object* GetObject() const { return &m_object; }

    void Update();
    void Draw();
    void UnInit();
};

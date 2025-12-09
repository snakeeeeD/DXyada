#pragma once


class StageBase {
public:
    virtual ~StageBase() {}
    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void UnInit() = 0;
};

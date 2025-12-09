#pragma once

class SceneManager;

class SceneBase {
public:
    virtual ~SceneBase() {}
    virtual void Init() = 0;
    virtual void Update(SceneManager& mgr) = 0;
    virtual void Draw() = 0;
    virtual void UnInit() = 0;
};

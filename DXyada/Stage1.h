#pragma once
#include <vector>
#include "Platform.h"
#include "Enemy.h"
#include "Texture.h"
#include"StageBase.h"



class Stage1 : public StageBase {
private:
    std::vector<Platform> m_platforms;
    std::vector<Enemy> m_enemy;
    Object m_background;
    Enemy enemy1;

public:
    void Init() override;
    void Update() override;
    void Draw() override;
    void UnInit() override;
    const std::vector<Platform>& GetPlatforms() const { return m_platforms; }
    const std::vector<Enemy>& GetEnemy() const { return m_enemy; }
};

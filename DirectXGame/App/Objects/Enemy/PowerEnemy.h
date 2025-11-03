#pragma once
#include "EnemyBase.h"

class PowerEnemy : public EnemyBase {
public:
	void Initialize(const EnemyData& data) override;
	void Update(const KamataEngine::Vector3& playerPos) override;

private:
	float attackCooldown_ = 0.0f;
};

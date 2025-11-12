#pragma once
#include "EnemyBase.h"

/// <summary>
/// パワータイプの敵
/// </summary>
class PowerEnemy : public EnemyBase {
public:
	PowerEnemy() = default;

	void Initialize(const EnemyData& data) override;
	void Update(const KamataEngine::Vector3& playerPos) override;

private:
	float attackCooldown_ = 0.0f;
};

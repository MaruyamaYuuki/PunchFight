#pragma once
#include "EnemyBase.h"

/// <summary>
/// 通常敵
/// </summary>
class NormalEnemy : public EnemyBase {
public:
	NormalEnemy() = default; // デフォルトコンストラクタ追加
	void Initialize(const EnemyData& data) override;
	void Update(const KamataEngine::Vector3& playerPos) override;

private:
	float attackTimer_ = 0.0f;
	bool isAttacking_ = false;
};

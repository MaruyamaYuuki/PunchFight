#pragma once
#include "EnemyBase.h"

class NormalEnemy : public EnemyBase {
public:
	void Update(const KamataEngine::Vector3 playerPos) override;

private:
	void Attack() override;

	float moveWaitTimer_ = 0.0f;
	int attackCount_ = 0;
};

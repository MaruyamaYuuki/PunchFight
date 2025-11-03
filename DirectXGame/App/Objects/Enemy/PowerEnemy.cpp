#include "PowerEnemy.h"
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void PowerEnemy::Initialize(const EnemyData& data) { EnemyBase::Initialize(data); }

void PowerEnemy::Update(const Vector3& playerPos) {
	Vector3 diff = playerPos - worldTransform_.translation_;
	float dist = sqrtf(diff.x * diff.x + diff.z * diff.z);

	if (dist > 4.0f) {
		diff = Normalize(diff);
		worldTransform_.translation_.x += diff.x * (speed_ * 0.5f);
		worldTransform_.translation_.z += diff.z * (speed_ * 0.5f);
	} else {
		attackCooldown_ += 1.0f / 60.0f;
		if (attackCooldown_ >= 3.0f) {
			// 重攻撃
			attackCooldown_ = 0.0f;
		}
	}

	worldTransform_.UpdateMatrix();
}

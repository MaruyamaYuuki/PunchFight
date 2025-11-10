#include "PowerEnemy.h"
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void PowerEnemy::Initialize(const EnemyData& data) { EnemyBase::Initialize(data); }

void PowerEnemy::Update(const Vector3& playerPos) {
	Vector3 diff = playerPos - worldTransform_.translation_;
	/*	float dist = sqrtf(diff.x * diff.x + diff.z * diff.z);

	if (dist > 2.5f) {
		diff = Normalize(diff);
		worldTransform_.translation_.x += diff.x * (speed_ * 0.5f);
		worldTransform_.translation_.z += diff.z * (speed_ * 0.5f);
	} else {
		attackCooldown_ += 1.0f / 60.0f;
		if (attackCooldown_ > 2.5f && attackCooldown_ < 3.0f) {
			attackBox_.active = true;
			attackBox_.pos = worldTransform_.translation_ + Vector3{diff.x * 0.8f, 0.2f, diff.z * 0.8f};
			attackBox_.size = {1.0f, 1.0f, 1.0f};
		} else if (attackCooldown_ >= 3.0f) {
			attackBox_.active = false;
			attackCooldown_ = 0.0f;
		}
	}*/

	worldTransform_.UpdateMatrix();
}

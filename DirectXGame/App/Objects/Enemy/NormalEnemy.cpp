#include "NormalEnemy.h"
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void NormalEnemy::Initialize(const EnemyData& data) { EnemyBase::Initialize(data); }

void NormalEnemy::Update(const Vector3& playerPos) {
	Vector3 diff = playerPos - worldTransform_.translation_;
	float dist = std::sqrt(diff.x * diff.x + diff.z * diff.z);

	if (dist > 2.0f && !isAttacking_) {
		diff = diff / dist; // 正規化
		worldTransform_.translation_.x += diff.x * speed_;
		worldTransform_.translation_.z += diff.z * speed_;
	} else {
		attackTimer_ += 1.0f / 60.0f;
		if (attackTimer_ > 0.5f && attackTimer_ < 1.5f) {
			isAttacking_ = true;
			attackBox_.active = true;
			attackBox_.pos = worldTransform_.translation_ + Vector3{diff.x * 0.5f, 0.1f, diff.z * 0.5f};
			attackBox_.size = {0.6f, 0.8f, 0.6f};
		} else if (attackTimer_ >= 2.0f) {
			isAttacking_ = false;
			attackTimer_ = 0.0f;
			attackBox_.active = false;
		}
	}

	worldTransform_.UpdateMatrix();
}

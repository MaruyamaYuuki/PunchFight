#include "NormalEnemy.h"
#include <cmath>
#include <iostream>

void NormalEnemy::Update(const KamataEngine::Vector3 playerPos) {
	EnemyBase::Update(playerPos);

	if (!isAttacking_) {
		// プレイヤーに近づく
		if (distanceToPlayer_ > 3.0f) {
			float dx = playerPos.x - worldTransform_.translation_.x;
			float dz = playerPos.z - worldTransform_.translation_.z;
			float len = sqrtf(dx * dx + dz * dz);
			worldTransform_.translation_.x += (dx / len) * speed_;
			worldTransform_.translation_.z += (dz / len) * speed_;
		} else {
			// 一定距離に来たら攻撃準備
			moveWaitTimer_ += deltTime_;
			if (moveWaitTimer_ > 0.7f) {
				isAttacking_ = true;
				moveWaitTimer_ = 0.0f;
				attackCount_ = 0;
			}
		}
	} else {
		Attack();
	}

	worldTransform_.UpdateMatrix();
}

void NormalEnemy::Attack() {
	attackTimer_ += deltTime_;
	if (attackTimer_ > 0.3f) {
		std::cout << "NormalEnemy Punch! Power: " << attackPower_ << std::endl;
		attackTimer_ = 0.0f;
		attackCount_++;

		if (attackCount_ >= 3) {
			isAttacking_ = false;
		}
	}
}

#include "NormalEnemy.h"
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void NormalEnemy::Initialize(const EnemyData& data) { 
	EnemyBase::Initialize(data); 

	RIdleTexture_ = TextureManager::Load("enemies/REnemy.png");
	RAttackTexture_ = TextureManager::Load("enemies/RPunch.png");

	LIdleTexture_ = TextureManager::Load("enemies/LEnemy.png");
	LAttackTexture_ = TextureManager::Load("enemies/LPunch.png");
}

void NormalEnemy::Update(const Vector3& playerPos) {

	// ===== ノックバック中は何もしない =====
	if (isKnockBack_ || isStan_) {
		EnemyBase::Update(playerPos);
		worldTransform_.UpdateMatrix();
		return;
	}

	// ===== プレイヤーとの距離計算 =====
	Vector3 toPlayer = playerPos - worldTransform_.translation_;
	float dist = std::sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y + toPlayer.z * toPlayer.z);

	// プレイヤー方向（左右）を決める
	if (fabs(toPlayer.x) > 0.01f) {
		facingDir_ = (toPlayer.x > 0) ? 1.0f : -1.0f;
	}

	const float ATTACK_RANGE = 1.2f;

	// ===== 攻撃中の処理 =====
	if (isAttacking_) {

		// 攻撃中は移動しない！（ここが重要）
		attackTimer_--;

		if (attackTimer_ <= 0) {
			// 攻撃終了
			isAttacking_ = false;
			attackHitBox_.active = false;

			// 攻撃したので必ずクールタイムに入る
			attackCooldownTimer_ = attackCooldown_;
		} else {
			// 攻撃中はヒットボックスを維持
			float hitOffsetX = 0.5f * facingDir_;
			Vector3 hitPos = worldTransform_.translation_ + Vector3{hitOffsetX, 0.1f, 0.0f};
			SetAttackHitBox(hitPos);
		}

		EnemyBase::Update(playerPos);
		worldTransform_.UpdateMatrix();
		return;
	}

	// ===== クールタイム中 =====
	if (attackCooldownTimer_ > 0) {
		attackCooldownTimer_--;
	}

	// ===== 攻撃中じゃない＆クールタイム中じゃない =====
	if (dist > ATTACK_RANGE) {
		float moveSpeed = 0.025f;

		// プレイヤー方向を正規化
		Vector3 dir = toPlayer;
		float len = std::sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

		if (len > 0.001f) {
			dir.x /= len;
			dir.z /= len;
		}

		// x,z 両方向へ移動
		worldTransform_.translation_.x += dir.x * moveSpeed;
		worldTransform_.translation_.z += dir.z * moveSpeed;
	} else {
		// ===== 攻撃可能範囲 =====
		if (attackCooldownTimer_ <= 0) {
			// 攻撃開始
			isAttacking_ = true;
			attackTimer_ = float(attackDuration_);

			float offsetX = 0.5f * facingDir_;
			Vector3 hitPos = worldTransform_.translation_ + Vector3{offsetX, 0.1f, 0};

			SetAttackHitBox(hitPos);

			// 攻撃した時点でクールタイム開始
			attackCooldownTimer_ = attackCooldown_;
		}
	}

    // 移動・攻撃などの状態判定
	if (hp_ <= 0)
		state_ = EnemyState::Dead;
	else if (isKnockBack_)
		state_ = EnemyState::Knockback;
	else if (isAttacking_)
		state_ = EnemyState::Attacking;
	else if (speed_ > 0.0f)
		state_ = EnemyState::Walking;
	else
		state_ = EnemyState::Idle;

	// 親クラス処理
	EnemyBase::Update(playerPos);
	worldTransform_.UpdateMatrix();
	UpdateTextures();
}

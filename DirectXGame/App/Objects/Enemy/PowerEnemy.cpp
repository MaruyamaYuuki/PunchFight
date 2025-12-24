#include "PowerEnemy.h"
#include <cmath>

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;

void PowerEnemy::Initialize(const EnemyData& data) { 
	EnemyBase::Initialize(data); 

	RIdleTexture_ = TextureManager::Load("enemies/powerEnemy/RPower.png");
	RWaitTexture_ = TextureManager::Load("enemies/powerEnemy/RHeadbutt1.png");
	RAttackTexture_ = TextureManager::Load("enemies/powerEnemy/RHeadbutt2.png");
	RTackleTexture_ = TextureManager::Load("enemies/powerEnemy/RTackle.png");
	RStunTexture_ = TextureManager::Load("enemies/powerEnemy/RStun.png");
	RWalkTexture_[0] = TextureManager::Load("enemies/powerEnemy/RWalk1.png");
	RWalkTexture_[1] = TextureManager::Load("enemies/powerEnemy/RWalk2.png");
	RWalkTexture_[2] = TextureManager::Load("enemies/powerEnemy/RWalk3.png");
	RWalkTexture_[3] = TextureManager::Load("enemies/powerEnemy/RWalk4.png");

	LIdleTexture_ = TextureManager::Load("enemies/powerEnemy/LPower.png");
	LWaitTexture_ = TextureManager::Load("enemies/powerEnemy/LHeadbutt1.png");
	LAttackTexture_ = TextureManager::Load("enemies/powerEnemy/LHeadbutt2.png");
	LTackleTexture_ = TextureManager::Load("enemies/powerEnemy/LTackle.png");
	LStunTexture_ = TextureManager::Load("enemies/powerEnemy/LStun.png");
	LWalkTexture_[0] = TextureManager::Load("enemies/powerEnemy/LWalk1.png");
	LWalkTexture_[1] = TextureManager::Load("enemies/powerEnemy/LWalk2.png");
	LWalkTexture_[2] = TextureManager::Load("enemies/powerEnemy/LWalk3.png");
	LWalkTexture_[3] = TextureManager::Load("enemies/powerEnemy/LWalk4.png");

	attackCooldownTimer_ = attackCooldown_;
}

void PowerEnemy::Update(const Vector3& playerPos, const std::vector<std::unique_ptr<EnemyBase>>& allEnemies) {

	// ===== ノックバック中・スタン中・ノックアウト中は何もしない =====
	if (isKnockBack_ || hp_ <= 0) {
		EnemyBase::Update(playerPos, allEnemies);
		isAttackMode_ = false;
		isAttacking_ = false;
		attackHitBox_.active = false;
		worldTransform_.UpdateMatrix();

		// 状態をDeadまたはKnockbackに設定
		if (isKnockBack_)
			state_ = EnemyState::Knockback;
		else if (hp_ <= 0)
			state_ = EnemyState::Dead;
		else if (isStun_)
			state_ = EnemyState::Stunned;

		return;
	}

	// ===== プレイヤーとの距離計算 =====
	Vector3 toPlayer = playerPos - worldTransform_.translation_;
	float dist = std::sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y + toPlayer.z * toPlayer.z);

	// プレイヤー方向（左右）を決める
	if (!isAttackMode_ && !isAttacking_) {
    	if (fabs(toPlayer.x) > 0.01f) {
    		facingDir_ = (toPlayer.x > 0) ? 1.0f : -1.0f;
    	}
	}

	// ===== 攻撃処理 =====
	AttackProcess(playerPos);

	// ===== 攻撃中じゃない＆クールタイム中じゃない =====
	if (dist > ATTACK_RANGE_ && !isAttackMode_) {
		MoveTowardPlayer(playerPos, allEnemies);
	}

	// 移動・攻撃などの状態判定
	if (isAttacking_)
		state_ = EnemyState::Attacking;
	else if (isAttackMode_)
		state_ = EnemyState::AttackWait;
	else if (speed_ > 0.0f)
		state_ = EnemyState::Walking;
	else
		state_ = EnemyState::Idle;

	// 親クラス処理
	EnemyBase::Update(playerPos, allEnemies);
	worldTransform_.UpdateMatrix();
}

void PowerEnemy::TackleAttack(const Vector3& playerPos) {

	Vector3 toPlayer = playerPos - worldTransform_.translation_;
	float dist = sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y + toPlayer.z * toPlayer.z);

	// ===== 攻撃モード突入 =====
	if (!isAttackMode_ && dist <= ATTACK_RANGE_) {
		isAttackMode_ = true;

		// 向きを固定する
		tackleDirX_ = (toPlayer.x >= 0) ? 1.0f : -1.0f;
		facingDir_ = tackleDirX_;
	}

	if (!isAttackMode_) {
		return;
	}

	// ===== 溜め開始 =====
	if (!isTackleCharging_ && !isTackling_) {
		isTackleCharging_ = true;
		tackleChargeTimer_ = tackleChargeTime_;
		return;
	}

	// ===== 溜め中 =====
	if (isTackleCharging_) {
		tackleChargeTimer_ -= deltaTime_;

		if (tackleChargeTimer_ <= 0.0f) {
			isTackleCharging_ = false;
			isTackling_ = true;
			isAttacking_ = true;

			tackleMoveTimer_ = tackleMoveTime_;

			// 自分自身を攻撃判定に
			attackHitBox_.active = true;
			attackHitBox_.pos = worldTransform_.translation_;
			attackHitBox_.size = hitBox_.size;
		}
		return;
	}

	// ===== タックル中 =====
	if (isTackling_) {
		tackleMoveTimer_ -= deltaTime_;

		// ★ 固定した向きで突進
		worldTransform_.translation_.x += tackleDirX_ * tackleSpeed_;

		attackHitBox_.pos = worldTransform_.translation_;

		if (tackleMoveTimer_ <= 0.0f) {
			isTackling_ = false;
			isAttacking_ = false;
			isAttackMode_ = false;

			attackHitBox_.active = false;
			attackCooldownTimer_ = attackCooldown_;
		}
	}
}

void PowerEnemy::AttackProcess(const KamataEngine::Vector3& playerPos) {
	if (useTackle_) {
		TackleAttack(playerPos);
	} else {
		DoNormalAttack(playerPos);
	}
}

void PowerEnemy::UpdateTextures() {
	// 共通処理
	EnemyBase::UpdateTextures();

	// 攻撃中だけ上書き
	if (state_ == EnemyState::Attacking) {
		if (attackType_ == AttackType::Normal) {
			textureHandle_ = (facingDir_ > 0) ? RAttackTexture_ : LAttackTexture_;
		} else if (attackType_ == AttackType::Tackle) {
			textureHandle_ = (facingDir_ > 0) ? RTackleTexture_ : LTackleTexture_;
		}
	}
}
